library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity communication is
    Port (
             clock : in std_logic;
             reset : in std_logic;
             left : in integer;
             right : in integer;
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
    signal readState : readStates := readIdle;
    signal readOffset : integer := 0;

    type sendMessages is (none, A2FD_PINGs, F2AD_ERR_UNKNOWN_CODEs);
    signal sendMessage : sendMessages := none;
    signal sendOffset : integer := 0;

    signal frontTrigger : integer := 0;
    signal backTrigger : integer := 0;

    signal txStbs : std_logic := '0';

begin

    txStb <= txStbs;

    readsendFA : process(clock, reset)
    begin
        if reset = '1' then
            readState <= readIdle;
            txStbs <= '0';
            sendMessage <= none;
            sendOffset <= 0;
        else
            if rising_edge(clock) then
                -- If read something
                if rxStb = '1' then
                    if readState = readIdle then
                        case rxData is
                            when A2FD_PING =>
                                sendMessage <= A2FD_PINGs; -- TODO Not so brutal
                            when others =>
                                sendMessage <= F2AD_ERR_UNKNOWN_CODEs;
                        end case;
                    end if;
                end if;

                -- If what was sent is acknowledged and there is still something to send
                if txStbs = '0' or txAck = '1' then
                    txStbs <= '1';
                    sendOffset <= sendOffset + 1;
                    case sendMessage is
                        when none =>
                            txStbs <= '0';
                            sendOffset <= 0;
                        when A2FD_PINGs =>
                            txData <= A2FD_PING;
                            sendMessage <= none;
                        when F2AD_ERR_UNKNOWN_CODEs =>
                            case sendOffset is
                                when 0 =>
                                    txData <= F2AD_ERR;
                                when others =>
                                    txData <= ERR_UNKNOWN_CODE;
                                    sendMessage <= none;
                            end case;
                    end case;
                end if;
            end if;
        end if;
    end process;

end Behavioral;
