-- Testbench automatically generated online
-- at http://vhdl.lapinoo.net
-- Generation date : 27.2.2018 12:52:54 GMT

library ieee;
use ieee.std_logic_1164.all;

entity hcsr04_tb is
    end hcsr04_tb;

architecture tb of hcsr04_tb is

    component hcSr04
        generic (
                    fFpga : INTEGER := 2_000_000 -- Reduce speed to increase simulation speed
                );
        port (
                 clk      : in std_logic;
                 reset    : in std_logic;
                 echo     : in std_logic;
                 distance : out integer;
                 trigger  : out std_logic;
                 start    : in std_logic;
                 finished : out std_logic
             );
    end component;

    signal clk      : std_logic;
    signal reset    : std_logic;
    signal echo     : std_logic;
    signal distance : integer;
    signal trigger  : std_logic;
    signal start    : std_logic;
    signal finished : std_logic;

    constant TbPeriod : time := 500 ns;
    signal TbClock : std_logic := '0';
    signal TbSimEnded : std_logic := '0';

begin

    dut : hcSr04 port map (clk      => clk,
                           reset    => reset,
                           echo     => echo,
                           distance => distance,
                           trigger  => trigger,
                           start    => start,
                           finished => finished);

    -- Clock generation
    TbClock <= not TbClock after TbPeriod/2 when TbSimEnded /= '1' else '0';

    clk <= TbClock;

    stimuli : process
        variable startT : time;
        variable stopT : time;
        variable delta : time;
    begin
        echo <= '0';
        start <= '0';

        -- Reset generation
        reset <= '1';
        wait for 100 ns;
        reset <= '0';
        wait for 100 ns;


        report "TEST First measurement" severity note;
        startT := now;
        start <= '1';
        wait for TbPeriod;
        start <= '0';

        wait for 19 ms;
        echo <= '1';
        wait for 15 ms;
        echo <= '0';

        wait on finished until finished = '1';
        stopT := now;

        delta := stopT - startT;
        report "Measurement took " & time'image(now);

        assert distance = 15000 report "Wrong distance reported" severity error;

        wait for 5 ms; -- Margin


        report "TEST Second measurement" severity note;
        startT := now;
        start <= '1';
        wait for TbPeriod;
        start <= '0';


        wait on finished until finished = '1';
        stopT := now;

        delta := stopT - startT;
        report "Measurement took " & time'image(now);

        assert distance = 0 report "Wrong distance reported" severity error;

        wait for 5 ms; -- Margin


        -- Stop the clock and hence terminate the simulation
        TbSimEnded <= '1';
        wait;
    end process;

end tb;
