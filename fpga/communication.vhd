library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity communication is
    Port (
             clock : in std_logic;
             reset : in std_logic;
             left : in integer;
             right : in integer;
             zerocoder : out std_logic := '0';
             front : in integer;
             back : in integer;
             txData : out std_logic_vector(7 downto 0) := x"00";
             txStb : out std_logic := '0';
             txAck : in std_logic;
             rxData : in std_logic_vector(7 downto 0);
             rxStb : in std_logic;
             enA : out std_logic_vector(7 downto 0) := x"00";
             in1enC : out std_logic_vector(7 downto 0) := x"00";
             in2 : out std_logic := '0';
             enB : out std_logic_vector(7 downto 0) := x"00";
             in3enD : out std_logic_vector(7 downto 0) := x"00";
             in4 : out std_logic := '0'
         );
end communication;

architecture Behavioral of communication is

    constant C2FD_PING : std_logic_vector(7 downto 0) := x"50"; -- 'P'
    constant C2FD_RESETCODER : std_logic_vector(7 downto 0) := x"52"; -- 'R'
    constant F2CD_ERR : std_logic_vector(7 downto 0) := x"45"; -- 'E'
    constant ERR_UNKNOWN_CODE : std_logic_vector(7 downto 0) := x"43"; -- 'C'
    constant F2CI_CODER : std_logic_vector(7 downto 0) := x"44"; -- 'D'
    constant F2CI_CAPT : std_logic_vector(7 downto 0) := x"43"; -- 'C'
    constant F2CT_CAPT : std_logic_vector(7 downto 0) := x"63"; -- 'c'
    constant C2FD_PWM : std_logic_vector(7 downto 0) := x"57"; -- 'W'
    constant C2FD_PWM2 : std_logic_vector(7 downto 0) := x"77"; -- 'w'

    signal frontTrigger : integer := 0;
    signal backTrigger : integer := 0;
    signal triggerSet : std_logic := '0';

    type readMessages is (F2CT_CAPTs, C2FD_PWMs, C2FD_PWM2s);
    signal readMessage : readMessages;
    signal readSize : integer := 0;
    signal readOffset : integer := 0;
    signal readData : std_logic_vector(63 downto 0) := (others => '0'); -- Max message size (will be trimmed down by the synthetizer)

    type sendMessages is (C2FD_PINGs, F2CI_CODERs, F2CI_CAPTs, F2CD_ERR_UNKNOWN_CODEs);
    constant SENDQUEUE_SIZE : integer := 16;
    type sendQueueMemorya is array (0 to SENDQUEUE_SIZE - 1) of sendMessages;
    signal sendQueueMemory : sendQueueMemorya;
    signal sendTail : integer := 0;
    signal sendHead : integer := 0;
    signal sendLooped : std_logic := '0';

    signal sendAvailable: std_logic := '0';
    signal sendBegun : std_logic := '0';
    signal sendOffset : integer := 0;
    signal sendSize : integer := 0;
    signal sendData : std_logic_vector(63 downto 0) := (others => '0'); -- Max message size (will be trimmed down by the synthetizer)

begin

    readsendFA : process(clock, reset)

        procedure pushSend
        (message : in sendMessages) is
        begin
            sendQueueMemory(sendHead) <= message;
            if sendHead < SENDQUEUE_SIZE - 1 then
                sendHead <= sendHead + 1;
            else
                sendHead <= 0;
                sendLooped <= '1';
            end if;
        end pushSend;

    begin
        if reset = '1' then
            frontTrigger <= 0;
            backTrigger <= 0;
            triggerSet <= '0';
            readSize <= 0;
            readOffset <= 0;
            readData <= (others => '0');
            sendTail <= 0;
            sendHead <= 0;
            sendLooped <= '0';
            sendAvailable<= '0';
            sendBegun <= '0';
            sendOffset <= 0;
            sendSize <= 0;
            sendData <= (others => '0');
            txStb <= '0';
            zerocoder <= '0';
            txData <= x"00";
        else
            if rising_edge(clock) then
                zerocoder <= '0';

                -- If read something
                if rxStb = '1' then -- Incomming character
                    if readSize = 0 then -- Beginning of message
                        readSize <= 0;
                        case rxData is
                            when C2FD_PING =>
                                pushSend(C2FD_PINGs);
                            when F2CI_CODER =>
                                pushSend(F2CI_CODERs);
                            when F2CI_CAPT =>
                                pushSend(F2CI_CAPTs);
                            when F2CT_CAPT =>
                                readMessage <= F2CT_CAPTs;
                                readSize <= 4;
                            when C2FD_PWM =>
                                readMessage <= C2FD_PWMs;
                                readSize <= 3;
                            when C2FD_PWM2 =>
                                readMessage <= C2FD_PWM2s;
                                readSize <= 4;
                            when others =>
                                pushSend(F2CD_ERR_UNKNOWN_CODEs);
                        end case;
                    else -- Rest of message
                        if readOffset < readSize then
                            readData((readOffset + 1) * 8 - 1 downto readOffset * 8) <= rxData;
                            readOffset <= readOffset + 1;
                        end if;
                    end if;
                elsif readSize > 0 and readOffset = readSize then -- Rest of message ended
                    case readMessage is
                        when F2CT_CAPTs =>
                            frontTrigger <= to_integer(unsigned(readData(15 downto 0)));
                            backTrigger <= to_integer(unsigned(readData(31 downto 16)));
                            triggerSet <= '1';
                        when C2FD_PWMs =>
                            enA <= readData(7 downto 0);
                            enB <= readData(15 downto 8);
                            if readData(16) = '1' then
                                in1enC <= x"FF";
                            else
                                in1enC <= x"00";
                            end if;
                            in2 <= readData(17);
                            if readData(18) = '1' then
                                in3enD <= x"FF";
                            else
                                in3enD <= x"00";
                            end if;
                            in4 <= readData(19);
                        when C2FD_PWM2s =>
                            enA <= readData(7 downto 0);
                            in1enC <= readData(15 downto 8);
                            enB <= readData(23 downto 16);
                            in3enD <= readData(31 downto 24);
                            in2 <= '0';
                            in4 <= '0';
                        when others =>
                            pushSend(F2CD_ERR_UNKNOWN_CODEs);
                    end case;
                    readOffset <= 0;
                    readSize <= 0;
                elsif (triggerSet = '1' and ((front > frontTrigger) or (back > backTrigger))) then
                    pushSend(F2CI_CAPTs);
                    triggerSet <= '0';
                end if;

                if sendAvailable = '0' then -- If no message is being sent
                    if sendTail < sendHead or sendLooped = '1' then -- If there is a message in the queue

                        -- Update tail
                        if sendTail < SENDQUEUE_SIZE - 1 then
                            sendTail <= sendTail + 1;
                        else
                            sendTail <= 0;
                            sendLooped <= '0';
                        end if;

                        sendAvailable <= '1';

                        case sendQueueMemory(sendTail) is
                            when C2FD_PINGs =>
                                sendData(7 downto 0) <= C2FD_PING;
                                sendSize <= 1;
                            when F2CI_CAPTs =>
                                sendData(7 downto 0) <= F2CI_CAPT;
                                sendData(23 downto 8) <= std_logic_vector(to_signed(front, 16));
                                sendData(39 downto 24) <= std_logic_vector(to_unsigned(back, 16));
                                sendSize <= 5;
                            when F2CI_CODERs =>
                                zerocoder <= '1';
                                sendData(7 downto 0) <= F2CI_CODER;
                                sendData(23 downto 8) <= std_logic_vector(to_signed(left, 16));
                                sendData(39 downto 24) <= std_logic_vector(to_signed(right, 16));
                                sendSize <= 5;
                            when others => -- Including F2CD_ERR_UNKNOWN_CODEs
                                sendData(7 downto 0) <= F2CD_ERR;
                                sendData(15 downto 8) <= ERR_UNKNOWN_CODE;
                                sendSize <= 2;
                        end case;
                    end if;

                else -- If a message is being sent
                    if sendBegun = '0' or txAck = '1' then
                        sendBegun <= '1';
                        if sendOffset < sendSize then -- There is still data to send
                            txData <= sendData((sendOffset + 1) * 8 - 1 downto sendOffset * 8);
                            txStb <= '1';
                            sendOffset <= sendOffset + 1;
                        else -- There is no more data to send
                            sendOffset <= 0;
                            txStb <= '0';
                            sendBegun <= '0';
                            sendAvailable <= '0';
                        end if;
                    end if;
                end if;

            end if;
        end if;
    end process;
end Behavioral;
