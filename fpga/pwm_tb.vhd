-- Testbench automatically generated online
-- at http://vhdl.lapinoo.net
-- Generation date : 1.5.2018 17:12:00 GMT

library ieee;
use ieee.std_logic_1164.all;

entity PWM_tb is
end PWM_tb;

architecture tb of PWM_tb is

    component PWM
        port (clk   : in std_logic;
              data  : in std_logic_vector (7 downto 0);
              pulse : out std_logic);
    end component;

    signal clk   : std_logic;
    signal data  : std_logic_vector (7 downto 0);
    signal pulse : std_logic;

    constant TbPeriod : time := 20 ns;
    signal TbClock : std_logic := '0';
    signal TbSimEnded : std_logic := '0';

begin

    dut : PWM
    port map (clk   => clk,
              data  => data,
              pulse => pulse);

    -- Clock generation
    TbClock <= not TbClock after TbPeriod/2 when TbSimEnded /= '1' else '0';

    clk <= TbClock;

    stimuli : process
    begin
        data <= x"00";
        wait for 100 * TbPeriod;

        data <= x"FF";
        wait for 100 * TbPeriod;

        data <= x"80";
        wait for 100 * TbPeriod;

        data <= x"e6";
        wait for 100 * TbPeriod;

        data <= x"3c";
        wait for 100 * TbPeriod;

        -- Stop the clock and hence terminate the simulation
        TbSimEnded <= '1';
        wait;
    end process;

end tb;
