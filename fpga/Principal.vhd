library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity Principal is
    Generic(
               fFpga : INTEGER := 50_000_000;
               fBaud : INTEGER := 115200
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
             BACKECHO: in std_logic;
             ENA: out std_logic;
             IN1ENC: out std_logic;
             IN2: out std_logic;
             ENB: out std_logic;
             IN3END: out std_logic;
             IN4: out std_logic
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

    -- Distance sensors
    signal front : integer := 0;
    signal frontRaw : integer := 0;
    signal frontFinished : std_logic;
    signal back : integer := 0;
    signal backRaw : integer := 0;
    signal backFinished : std_logic;
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
    component fir is
        Port (
                 clock : in STD_LOGIC;
                 reset : in STD_LOGIC;
                 signalIn : in INTEGER;
                 signalOut : out INTEGER;
                 start : in STD_LOGIC;
                 done : out STD_LOGIC
             );
    end component;

    -- PWM clock
    signal pwmClk : std_logic := '0';
    signal pwmCounter : integer := 0;
    constant PWM_DIVIDER : integer := 1024;

    -- Motor controller
    signal enAd : std_logic_vector(7 downto 0);
    signal in1enCd : std_logic_vector(7 downto 0);
    signal in2d : std_logic;
    signal enBd : std_logic_vector(7 downto 0);
    signal in3enDd : std_logic_vector(7 downto 0);
    signal in4d : std_logic;
    component PWM is
        port (
                 clk : in std_logic;
                 data : in std_logic_vector (7 downto 0);
                 pulse : out std_logic
             );
    end component;

    -- CF
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
                 rxStb : in std_logic;
                 enA : out std_logic_vector(7 downto 0);
                 in1enC : out std_logic_vector(7 downto 0);
                 in2 : out std_logic;
                 enB : out std_logic_vector(7 downto 0);
                 in3enD : out std_logic_vector(7 downto 0);
                 in4 : out std_logic
             );
    end component;

begin

    reset <= BTN;

    pwmClkGenerator: process (clk) begin
        if rising_edge(clk) then
            if (pwmCounter >= PWM_DIVIDER) then
                pwmClk <= not pwmClk;
                pwmCounter <= 0;
            else
                pwmCounter <= pwmCounter + 1;
            end if;
        end if;
    end process;

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
                                   distance => frontRaw,
                                   trigger => FRONTTRIGGER,
                                   start => '1',
                                   finished => frontFinished
                               );
    frontFilter : FIR port map (
                                   clock     => CLK,
                                   reset     => reset,
                                   signalIn  => frontRaw,
                                   signalOut => front,
                                   start     => frontFinished
                               -- done      => done
                               );

    backCapt: hcsr04 port map (
                                  clk => CLK,
                                  reset => reset,
                                  echo => BACKECHO,
                                  distance => backRaw,
                                  trigger => BACKTRIGGER,
                                  start => '1',
                                  finished => backFinished
                              );
    backFilter : FIR port map (
                                  clock     => CLK,
                                  reset     => reset,
                                  signalIn  => backRaw,
                                  signalOut => back,
                                  start     => backFinished
                              -- done      => done
                              );
    enAp : PWM port map (
                            clk => pwmClk,
                            data => enAd,
                            pulse => ENA
                        );

    in1enCp : PWM port map (
                               clk => pwmClk,
                               data => in1enCd,
                               pulse => IN1ENC
                           );
    IN2 <= in2d;

    enBp : PWM port map (
                            clk => pwmClk,
                            data => enBd,
                            pulse => ENB
                        );

    in3enDp : PWM port map (
                               clk => pwmClk,
                               data => in3enDd,
                               pulse => IN3END
                           );
    IN4 <= in4d;


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
                                   rxStb  => rxStb,
                                   enA    => enAd,
                                   in1enC => in1enCd,
                                   in2    => in2d,
                                   enB    => enBd,
                                   in3enD => in3enDd,
                                   in4    => in4d
                               );
end Behavioral;

