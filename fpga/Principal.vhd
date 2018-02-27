library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity Principal is
    Generic(
               fFpga : INTEGER := 50_000_000;
               fBaud : INTEGER := 9600
           );
    Port (
             CLK : in  std_logic;
             BTN: in std_logic;
             RX: in std_logic;
             TX: out std_logic;
             LEFTCHA: in std_logic;
             LEFTCHB: in std_logic;
             RIGHTCHA: in std_logic;
             RIGHTCHB: in std_logic;
             FRONTTRIGGER: out std_logic;
             FRONTECHO: in std_logic;
             BACKTRIGGER: out std_logic;
             BACKECHO: in std_logic
         );
end Principal;

architecture Behavioral of Principal is
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
    component hcsr04 IS
        generic(
                   fFpga : INTEGER := fFpga
               );
        port(
                clk : IN STD_LOGIC;
                reset : IN STD_LOGIC;
                echo : IN STD_LOGIC;
                distance : OUT INTEGER;
                trigger : OUT STD_LOGIC;
                start : IN STD_LOGIC;
                finished : OUT STD_LOGIC
            );
    end component;

    -- AF
    component uart is
        generic (
                    baud                : positive := fBaud;
                    clock_frequency     : positive := fFpga
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

begin

    reset <= BTN;

    leftCoder: hedm port map (
                                 clk => CLK,
                                 chA => LEFTCHA,
                                 chB => LEFTCHB,
                                 reset => reset,
                                 zero => zerocoder,
                                 counts => left
                             );

    rightCoder: hedm port map (
                                  clk => CLK,
                                  chA => RIGHTCHA,
                                  chB => RIGHTCHB,
                                  reset => reset,
                                  zero => zerocoder,
                                  counts => right
                              );
    frontCapt: hcsr04 port map (
                                   clk => CLK,
                                   reset => reset,
                                   echo => FRONTECHO,
                                   distance => front,
                                   trigger => FRONTTRIGGER,
                                   start => '1'
                               -- finished =>
                               );

    backCapt: hcsr04 port map (
                                  clk => CLK,
                                  reset => reset,
                                  echo => BACKECHO,
                                  distance => back,
                                  trigger => BACKTRIGGER,
                                  start => '1'
                              -- finished =>
                              );

    FA: uart port map(
                         clock               => CLK,
                         reset               => reset,
                         data_stream_in      => txData,
                         data_stream_in_stb  => txStb,
                         data_stream_in_ack  => txAck,
                         data_stream_out     => rxData,
                         data_stream_out_stb => rxStb,
                         tx                  => TX,
                         rx                  => RX
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
end Behavioral;

