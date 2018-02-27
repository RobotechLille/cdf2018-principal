library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity communication is
    Port (
             clock : in std_logic;
             reset : in std_logic;
             left : in integer;
             right : in integer;
             zerocoder : out std_logic;
             front : in integer;
             back : in integer;
             txData : out std_logic_vector(7 downto 0);
             txStb : out std_logic;
             txAck : in std_logic;
             rxData : in std_logic_vector(7 downto 0);
             rxStb : in std_logic
         );
end communication;

architecture Behavioral of communication is

    constant A2FD_PING : std_logic_vector(7 downto 0) := x"50"; -- 'P'
    constant A2FD_RESETCODER : std_logic_vector(7 downto 0) := x"52"; -- 'R'
    constant F2AD_ERR : std_logic_vector(7 downto 0) := x"45"; -- 'E'
    constant ERR_UNKNOWN_CODE : std_logic_vector(7 downto 0) := x"43"; -- 'C'
    constant F2AI_CODER : std_logic_vector(7 downto 0) := x"44"; -- 'D'
    constant F2AI_CAPT : std_logic_vector(7 downto 0) := x"43"; -- 'C'
    constant F2AT_CAPT : std_logic_vector(7 downto 0) := x"63"; -- 'c'

    type readStates is (readIdle);
    signal readState : readStates := readIdle; -- TODO Make sure is correctly reset when reworking this
    signal readOffset : integer := 0;

    type sendMessages is (none, A2FD_PINGs, F2AI_CODERs, F2AI_CAPTs, F2AD_ERR_UNKNOWN_CODEs);

    constant SENDQUEUE_SIZE : integer := 16;
    type sendQueueMemorya is array (0 to SENDQUEUE_SIZE - 1) of sendMessages;

    signal frontTrigger : integer := 0;
    signal backTrigger : integer := 0;

    signal txStbs : std_logic := '0';

begin

    txStb <= txStbs;

    readsendFA : process(clock, reset)

        variable sendMessage : sendMessages := none;
        variable sendOffset : integer := 0;
        variable sendSize : integer := 0;
        variable sendData : std_logic_vector(63 downto 0); -- Max message size (will be trimmed down by the synthetizer)

        -- Send queue
        variable sendQueueMemory : sendQueueMemorya;
        variable sendTail : integer := 0;
        variable sendHead : integer := 0;
        variable sendLooped : boolean := false;

        procedure pushSend
        (message : in sendMessages) is
        begin
            sendQueueMemory(sendHead) := message;
            if sendHead < SENDQUEUE_SIZE - 1 then
                sendHead := sendHead + 1;
            else
                sendHead := 0;
                sendLooped := true;
            end if;
        end pushSend;

        procedure popSend
        (message : out sendMessages) is
        begin
            if sendTail < sendHead or sendLooped then
                message := sendQueueMemory(sendTail);
                if sendTail < SENDQUEUE_SIZE - 1 then
                    sendTail := sendTail + 1;
                else
                    sendTail := 0;
                    sendLooped := false;
                end if;
            else
                message := none;
            end if;
        end popSend;

    begin
        if reset = '1' then
            readState <= readIdle;
            sendMessage := none;
            sendOffset := 0;
            sendSize := 0;
            sendTail := 0;
            sendHead := 0;
            sendLooped := false;
            frontTrigger <= 0;
            backTrigger <= 0;
            zerocoder <= '0';
            txData <= x"00";
        else
            if rising_edge(clock) then
                zerocoder <= '0';

                -- If read something
                if rxStb = '1' then
                    if readState = readIdle then
                        case rxData is
                            when A2FD_PING =>
                                pushSend(A2FD_PINGs);
                            when F2AI_CODER =>
                                pushSend(F2AI_CODERs);
                            when F2AI_CAPT =>
                                pushSend(F2AI_CAPTs);
                            when others =>
                                pushSend(F2AD_ERR_UNKNOWN_CODEs);
                        end case;
                    end if;
                end if;

                -- If what was sent is acknowledged or nothing is being sent atm
                if txStbs = '0' or txAck = '1' then
                    if sendSize = 0 then -- If no data to be sent
                        popSend(sendMessage); -- See if there a message in the message queue
                        case sendMessage is
                            when none => -- No message available, do nothing
                            when A2FD_PINGs =>
                                sendData(7 downto 0) := A2FD_PING;
                                sendSize := 1;
                            when F2AI_CAPTs =>
                                sendData(7 downto 0) := F2AI_CAPT;
                                sendData(23 downto 8) := std_logic_vector(to_signed(front, 16));
                                sendData(39 downto 24) := std_logic_vector(to_unsigned(back, 16));
                                sendSize := 5;
                            when F2AI_CODERs =>
                                zerocoder <= '1';
                                sendData(7 downto 0) := F2AI_CODER;
                                sendData(23 downto 8) := std_logic_vector(to_signed(left, 16));
                                sendData(39 downto 24) := std_logic_vector(to_signed(right, 16));
                                sendSize := 5;
                            when others => -- Including F2AD_ERR_UNKNOWN_CODEs
                                sendData(7 downto 0) := F2AD_ERR;
                                sendData(15 downto 8) := ERR_UNKNOWN_CODE;
                                sendSize := 2;
                        end case;
                    end if;

                    if sendSize > 0 then -- If data to be sent
                        txData <= sendData((sendOffset + 1) * 8 - 1 downto sendOffset * 8);
                        txStbs <= '1';

                        if sendOffset = sendSize - 1 then -- If it was the last character sent
                            sendSize := 0; -- Make next iteration check for send queue
                            sendOffset := 0;
                        else -- Still data to be sent after that
                            sendOffset := sendOffset + 1;
                        end if;
                    else -- If really no data to be sent
                        txStbs <= '0';
                    end if;
                end if;
            end if;
        end if;
    end process;

end Behavioral;
