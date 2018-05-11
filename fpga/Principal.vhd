library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity Principal is
    Generic(
               fFpga : INTEGER := 50_000_000
           );
    Port (
             CLK : in  std_logic;
             BTN: in std_logic;
             SDA: inout std_logic;
             SCL: inout std_logic;
             LEFTCHA: in std_logic;
             LEFTCHB: in std_logic;
             RIGHTCHA: in std_logic;
             RIGHTCHB: in std_logic;
             FRONTTRIGGER: out std_logic;
             BACKTRIGGER: out std_logic;
             FRONTLECHO: in std_logic;
             BACKLECHO: in std_logic;
             FRONTRECHO: in std_logic;
             BACKRECHO: in std_logic;
             ENAREF: out std_logic;
             ENA: out std_logic;
             IN1: out std_logic;
             IN2: out std_logic;
             ENBREF: out std_logic;
             ENB: out std_logic;
             IN3: out std_logic;
             IN4: out std_logic
         );
end Principal;

architecture Behavioral of Principal is
    -- General
    signal reset : std_logic := '0';

    -- Encoder
    signal left : integer;
    signal right : integer;

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
    signal frontMin : integer := 0;
    signal backMin : integer := 0;
    signal frontL : integer := 0;
    signal frontLRaw : integer := 0;
    signal frontLFinished : std_logic;
    signal backL : integer := 0;
    signal backLRaw : integer := 0;
    signal backLFinished : std_logic;
    signal frontR : integer := 0;
    signal frontRRaw : integer := 0;
    signal frontRFinished : std_logic;
    signal backR : integer := 0;
    signal backRRaw : integer := 0;
    signal backRFinished : std_logic;
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
    constant PWM_DIVIDER : integer := 4096;

    -- Motor controller
    signal enAd : std_logic_vector(7 downto 0);
    signal enBd : std_logic_vector(7 downto 0);
    signal ind : std_logic_vector(7 downto 0);

    component PWM is
        port (
                 clk : in std_logic;
                 data : in std_logic_vector (7 downto 0);
                 pulse : out std_logic
             );
    end component;

    -- CF
    component I2CSLAVE
        generic(
                   DEVICE: std_logic_vector(7 downto 0) := x"42"
               );
        port(
                MCLK        : in    std_logic;
                nRST        : in    std_logic;
                SDA_IN      : in    std_logic;
                SCL_IN      : in    std_logic;
                SDA_OUT     : out   std_logic;
                SCL_OUT     : out   std_logic;
                ADDRESS     : out   std_logic_vector(7 downto 0);
                DATA_OUT    : out   std_logic_vector(7 downto 0);
                DATA_IN     : in    std_logic_vector(7 downto 0);
                WR          : out   std_logic;
                RD          : out   std_logic
            );
    end component;
    signal sdaIn : std_logic;
    signal sclIn : std_logic;
    signal sdaOut : std_logic;
    signal sclOut : std_logic;
    signal address : std_logic_vector(7 downto 0);
    signal dataOut : std_logic_vector(7 downto 0);
    signal dataIn : std_logic_vector(7 downto 0);
    signal wr : std_logic;
    signal rd : std_logic;
    signal rdP : std_logic;


    signal leftB : std_logic_vector(15 downto 0);
    signal rightB : std_logic_vector(15 downto 0);
    signal frontLRawB : std_logic_vector(15 downto 0);
    signal frontRRawB : std_logic_vector(15 downto 0);
    signal backLRawB : std_logic_vector(15 downto 0);
    signal backRRawB : std_logic_vector(15 downto 0);
    signal frontLB : std_logic_vector(15 downto 0);
    signal frontRB : std_logic_vector(15 downto 0);
    signal backLB : std_logic_vector(15 downto 0);
    signal backRB : std_logic_vector(15 downto 0);

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
                                 zero => '0',
                                 counts => left
                             );

    rightCoder: hedm port map (
                                  clk => CLK,
                                  chA => RIGHTCHA,
                                  chB => RIGHTCHB,
                                  reset => reset,
                                  zero => '0',
                                  counts => right
                              );

    frontLCapt: hcsr04 port map (
                                    clk => CLK,
                                    reset => reset,
                                    echo => FRONTLECHO,
                                    distance => frontLRaw,
                                    trigger => FRONTTRIGGER,
                                    start => '1',
                                    finished => frontLFinished
                                );
    frontLFilter : FIR port map (
                                    clock     => CLK,
                                    reset     => reset,
                                    signalIn  => frontLRaw,
                                    signalOut => frontL,
                                    start     => frontLFinished
                                -- done      => done
                                );

    frontRCapt: hcsr04 port map (
                                    clk => CLK,
                                    reset => reset,
                                    echo => FRONTRECHO,
                                    distance => frontRRaw,
                                    -- trigger => FRONTTRIGGER,
                                    start => '1',
                                    finished => frontRFinished
                                );
    frontRFilter : FIR port map (
                                    clock     => CLK,
                                    reset     => reset,
                                    signalIn  => frontRRaw,
                                    signalOut => frontR,
                                    start     => frontRFinished
                                -- done      => done
                                );

    backLCapt: hcsr04 port map (
                                   clk => CLK,
                                   reset => reset,
                                   echo => BACKLECHO,
                                   distance => backLRaw,
                                   trigger => BACKTRIGGER,
                                   start => '1',
                                   finished => backLFinished
                               );
    backLFilter : FIR port map (
                                   clock     => CLK,
                                   reset     => reset,
                                   signalIn  => backLRaw,
                                   signalOut => backL,
                                   start     => backLFinished
                               -- done      => done
                               );
    backRCapt: hcsr04 port map (
                                   clk => CLK,
                                   reset => reset,
                                   echo => BACKRECHO,
                                   distance => backRRaw,
                                   -- trigger => BACKTRIGGER,
                                   start => '1',
                                   finished => backRFinished
                               );
    backRFilter : FIR port map (
                                   clock     => CLK,
                                   reset     => reset,
                                   signalIn  => backRRaw,
                                   signalOut => backR,
                                   start     => backRFinished
                               -- done      => done
                               );
    enAp : PWM port map (
                            clk => pwmClk,
                            data => enAd,
                            pulse => ENA
                        );
    ENAREF <= '1';

    IN1 <= ind(0);
    IN2 <= ind(1);

    enBp : PWM port map (
                            clk => pwmClk,
                            data => enBd,
                            pulse => ENB
                        );
    ENBREF <= '1';

    IN3 <= ind(2);
    IN4 <= ind(3);

    FA : i2cslave port map (
                               MCLK        => clk,
                               nRST        => not reset,
                               SDA_IN      => sdaIn,
                               SCL_IN      => sclIn,
                               SDA_OUT     => sdaOut,
                               SCL_OUT     => sclOut,
                               ADDRESS     => address,
                               DATA_OUT    => dataOut,
                               DATA_IN     => dataIn,
                               WR          => wr,
                               RD          => rd
                           );

    SCL <= 'Z' when sclOut = '1' else '0';
    sclIn <= to_UX01(SCL);
    SDA <= 'Z' when sdaOut = '1' else '0';
    sdaIn <= to_UX01(SDA);

    leftB <= std_logic_vector(to_signed(left, 16));
    rightB <= std_logic_vector(to_signed(right, 16));
    frontLRawB <= std_logic_vector(to_unsigned(frontLRaw, 16));
    frontRRawB <= std_logic_vector(to_unsigned(frontRRaw, 16));
    backLRawB <= std_logic_vector(to_unsigned(backLRaw, 16));
    backRRawB <= std_logic_vector(to_unsigned(backRRaw, 16));
    frontLB <= std_logic_vector(to_unsigned(frontL, 16));
    frontRB <= std_logic_vector(to_unsigned(frontR, 16));
    backLB <= std_logic_vector(to_unsigned(backL, 16));
    backRB <= std_logic_vector(to_unsigned(backR, 16));

    dataIn <=  x"50" when address = x"00" else
               leftB(15 downto 8) when address = x"10" else
               leftB(7 downto 0) when address = x"11" else
               rightB(15 downto 8) when address = x"12" else
               rightB(7 downto 0) when address = x"13" else
               frontLRawB(15 downto 8) when address = x"20" else
               frontLRawB(7 downto 0) when address = x"21" else
               frontRRawB(15 downto 8) when address = x"22" else
               frontRRawB(7 downto 0) when address = x"23" else
               backLRawB(15 downto 8) when address = x"24" else
               backLRawB(7 downto 0) when address = x"25" else
               backRRawB(15 downto 8) when address = x"26" else
               backRRawB(7 downto 0) when address = x"27" else
               frontLB(15 downto 8) when address = x"30" else
               frontLB(7 downto 0) when address = x"31" else
               frontRB(15 downto 8) when address = x"32" else
               frontRB(7 downto 0) when address = x"33" else
               backLB(15 downto 8) when address = x"34" else
               backLB(7 downto 0) when address = x"35" else
               backRB(15 downto 8) when address = x"36" else
               backRB(7 downto 0) when address = x"37" else
               ind when address = x"60" else
               enAd when address = x"61" else
               enBd when address = x"62" else
               (others => '0');

    ind <= dataOut when (address = x"60" and wr = '1') else ind;
    enAd <= dataOut when (address = x"61" and wr = '1') else enAd;
    enBd <= dataOut when (address = x"62" and wr = '1') else enBd;


end Behavioral;

