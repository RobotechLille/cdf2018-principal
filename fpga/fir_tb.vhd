-- Testbench automatically generated online
-- at http://vhdl.lapinoo.net
-- Generation date : 28.2.2018 08:45:52 GMT

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity fir_tb is
    end fir_tb;

architecture tb of fir_tb is

    component FIR
        port (clock     : in std_logic;
              reset     : in std_logic;
              signalIn  : in integer;
              signalOut : out integer;
              start     : in std_logic;
              done      : out std_logic);
    end component;

    signal clock     : std_logic;
    signal reset     : std_logic;
    signal signalIn  : integer;
    signal signalOut : integer;
    signal start     : std_logic;
    signal done      : std_logic;

    constant TbPeriod : time := 20 ns;
    signal TbClock : std_logic := '0';
    signal TbSimEnded : std_logic := '0';

    constant SignalPeriod : time := 100 us;
    constant SignalAmpl : integer := 1000;
    constant SamplingPeriod : time := TbPeriod * 150;
    constant TimeBase : time := 1 ns; -- For working with integers when generating the signal

begin

    dut : FIR
    port map (clock     => clock,
              reset     => reset,
              signalIn  => signalIn,
              signalOut => signalOut,
              start     => start,
              done      => done);

    -- Clock generation
    TbClock <= not TbClock after TbPeriod/2 when TbSimEnded /= '1' else '0';

    clock <= TbClock;

    sampling : process
        variable nowI : integer;
        variable SignalPeriodI : integer;
        variable x : integer;
        variable y : integer;
        variable z : integer;
    begin
        while TbSimEnded = '0' loop
            -- Not optimised at all... No worries though it's just a testbench
            nowI := now / TimeBase;
            SignalPeriodI := SignalPeriod / TimeBase;
            x := nowI rem SignalPeriodI;
            y := x * SignalAmpl;
            z := y / SignalPeriodI;
            signalIn <= z;
            start <= '1';
            wait for TbPeriod;
            start <= '0';
            wait for SamplingPeriod - TbPeriod;
        end loop;
        wait;
    end process;

    stimuli : process
    begin
        -- Reset generation
        reset <= '1';
        wait for 100 ns;
        reset <= '0';
        wait for 100 ns;

        wait for 5 * SignalPeriod;

        -- Stop the clock and hence terminate the simulation
        TbSimEnded <= '1';
        wait;
    end process;

end tb;
