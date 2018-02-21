library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity Principal is
    Port ( CLK : in  STD_LOGIC; -- Clock
           BTN : in  STD_LOGIC; -- Reset
                                -- FA
           IO : inout  STD_LOGIC_VECTOR (21 downto 20);
           -- Debug
           LED : out  STD_LOGIC_VECTOR (3 downto 0);
           AN : out  STD_LOGIC_VECTOR (3 downto 0);
           A_TO_G : out  STD_LOGIC_VECTOR (6 downto 0);
           DOT : out  STD_LOGIC
       );
end Principal;

architecture Behavioral of Principal is
    -- Blink led
    signal pulse : std_logic := '0';
    signal count : integer := 0;

    -- General
    signal reset : std_logic := '0';

    -- Encoder
    signal left : integer;
    signal right : integer;

    -- Sensors
    signal front : integer;
    signal back : integer;
    signal frontTrigger : integer := 0;
    signal backTrigger : integer := 0;

    -- AF
    component uart is
        generic (
                    baud                : positive := 9600;
                    clock_frequency     : positive := 50_000_000
                );
        port (
                 clock               :   in  std_logic;
                 reset               :   in  std_logic;
                 data_stream_in      :   in  std_logic_vector(7 downto 0);
                 data_stream_in_stb  :   in  std_logic;
                 data_stream_in_ack  :   out std_logic;
                 data_stream_out     :   out std_logic_vector(7 downto 0);
                 data_stream_out_stb :   out std_logic;
                 tx                  :   out std_logic;
                 rx                  :   in  std_logic
             );
    end component;

    constant BAUD_COUNT: std_logic_vector := x"1458"; -- 96000 Baud at 50 MHz

    signal txData : std_logic_vector(7 downto 0);
    signal txStb : std_logic := '0';
    signal txAck : std_logic := '0';

    signal rxData : std_logic_vector(7 downto 0);
    signal rxStb : std_logic := '0';

    constant A2FD_PING : std_logic_vector := x"50"; -- 'P'

    type readStates is (readIdle);
    signal readState : readStates := readIdle;

    type sendMessages is (none, A2FD_PINGs);
    signal resetSendMessageRead : std_logic := '0';
    signal resetSendMessageSend : std_logic := '0';
    signal sendMessage : sendMessages := none;
    signal sendOffset : integer := 0;

    -- Debug
    component sevenseg is
        Port (
                 data : in  STD_LOGIC_VECTOR (15 downto 0);
                 clock : in STD_LOGIC;
                 anode : out  STD_LOGIC_VECTOR (3 downto 0);
                 segment : out  STD_LOGIC_VECTOR (6 downto 0);
                 dot : out STD_LOGIC
             );
    end component;
    signal sevensegdata: std_logic_vector(15 downto 0);
    signal fullseg: std_logic_vector(7 downto 0);
begin

    reset <= BTN;

    FA: uart port map(
                         clock               => CLK,
                         reset               => reset,
                         data_stream_in      => txData,
                         data_stream_in_stb  => txStb,
                         data_stream_in_ack  => txAck,
                         data_stream_out     => rxData,
                         data_stream_out_stb => rxStb,
                         tx                  => IO(21),
                         rx                  => IO(20)
                     );

    readsendFA : process(reset, rxStb, txAck)
    begin
        if reset = '1' then
            readState <= readIdle;
            sendMessage <= none;
            txStb <= '0';
            sendOffset <= 0;
        else
            -- If read something
            if rxStb = '1' then
                if readState = readIdle then
                    case rxData is
                        when A2FD_PING => -- 'P'
                            sendMessage <= A2FD_PINGs; -- TODO Not so brutal
                        when others =>
                    end case;
                end if;
            end if;

            -- Reset sending if UART module  has begun sending (and has a copy of the byte)
            if txAck = '1' then
                txStb <= '0';
            end if;

            -- If what was sent is acknowledged and there is still something to send
            if txStb = '0' then
                case sendMessage is
                    when none =>
                    when A2FD_PINGs =>
                        txData <= A2FD_PING;
                        txStb <= '1';
                        sendMessage <= none;
                end case;
            end if;
        end if;
    end process;

    -- Debug
    blinkled : process(CLK, reset)
    begin
        if reset = '1' then
            count <= 0;
            pulse <= '0';
        elsif CLK'event and CLK = '1' then
            if count = 9999999 then
                count <= 0;
                pulse <= not pulse;
            else
                count <= count + 1;
            end if;
        end if;
    end process;
    LED(3) <= pulse;

    LED(2) <= txStb;
    LED(1) <= rxStb;
    LED(0) <= txAck;

    debugSeg: sevenseg port map(
                                   data => sevensegdata,
                                   clock => CLK,
                                   anode => AN,
                                   segment => A_TO_G,
                                   dot => DOT
                               );
    sevensegdata(15 downto 8) <= rxData;
    sevensegdata(7 downto 0) <= txData;

end Behavioral;

