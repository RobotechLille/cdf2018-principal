library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity Principal is
    Port ( CLK : in  STD_LOGIC; -- Clock
           BTN : in  STD_LOGIC; -- Reset

           -- FA & Encoder
           IO : inout  STD_LOGIC_VECTOR (21 downto 16);
           -- Debug
           LED : out  STD_LOGIC_VECTOR (3 downto 0);
           AN : out  STD_LOGIC_VECTOR (3 downto 0);
           A_TO_G : out  STD_LOGIC_VECTOR (6 downto 0);
           DOT : out  STD_LOGIC
       );
end Principal;

architecture Behavioral of Principal is
    -- Blink led
    signal count : integer := 0;
    signal theled: std_logic_vector(3 downto 0) := "0000";

    -- General
    signal reset : std_logic := '0';

    -- Encoder
    signal left : integer;
    signal right : integer;
    signal zerocoder : std_logic;

    component hedm is
        Port (
                 clk : in STD_LOGIC;
                 chA : in STD_LOGIC;
                 chB : in STD_LOGIC;
                 reset : in STD_LOGIC;
                 zero : in STD_LOGIC;
                 counts : out integer
             );
    end component;

    -- Sensors
    signal front : integer := 0;
    signal back : integer := 0;

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

    -- Handling
    component communication is
        Port (
                 clock : in  std_logic;
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
    end component;

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

    leftCoder: hedm port map (
                 clk => CLK,
                 chA => IO(19),
                 chB => IO(18),
                 reset => reset,
                 zero => zerocoder,
                 counts => left
             );

    rightCoder: hedm port map (
                 clk => CLK,
                 chA => IO(17),
                 chB => IO(16),
                 reset => reset,
                 zero => zerocoder,
                 counts => right
             );

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

    com: communication port map(
                                   clock => CLK,
                                   reset  => reset,
                                   left  => left,
                                   right  => right,
                                   zerocoder  => zerocoder,
                                   front  => front,
                                   back  => back,
                                   txData  => txData,
                                   txStb  => txStb,
                                   txAck  => txAck,
                                   rxData  => rxData,
                                   rxStb  => rxStb
                               );


    -- Debug
    blinkled : process(CLK, reset)
    begin
        if reset = '1' then
            count <= 0;
            theled <= "0000";
            front <= 0;
            back <= 0;
        elsif CLK'event and CLK = '1' then
            if count = 9999999 then
                count <= 0;
                theled(3) <= not theled(3);
                theled(2 downto 0) <= "000";
            else
                count <= count + 1;
                theled(2 downto 0)  <= theled(2 downto 0) or (txStb & rxStb & txAck);
            end if;

        end if;
    end process;

    LED <= theled;

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

