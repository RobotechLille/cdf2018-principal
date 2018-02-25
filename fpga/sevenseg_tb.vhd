-- Testbench automatically generated online
-- at http://vhdl.lapinoo.net
-- Generation date : 25.2.2018 12:42:48 GMT

library ieee;
use ieee.std_logic_1164.all;

entity sevenseg_tb is
end sevenseg_tb;

architecture tb of sevenseg_tb is

    component sevenseg
        port (data    : in std_logic_vector (15 downto 0);
              clock   : in std_logic;
              anode   : out std_logic_vector (3 downto 0);
              segment : out std_logic_vector (6 downto 0);
              dot     : out std_logic);
    end component;

    signal data    : std_logic_vector (15 downto 0);
    signal clock   : std_logic;
    signal anode   : std_logic_vector (3 downto 0);
    signal segment : std_logic_vector (6 downto 0);
    signal dot     : std_logic;

    constant TbPeriod : time := 20 ns;
    signal TbClock : std_logic := '0';
    signal TbSimEnded : std_logic := '0';

    constant SegPeriod : time := 4 ms;
    constant FourSegPeriod : time := 4 * SegPeriod;

begin

    dut : sevenseg
    port map (data    => data,
              clock   => clock,
              anode   => anode,
              segment => segment,
              dot     => dot);

    -- Clock generation
    TbClock <= not TbClock after TbPeriod/2 when TbSimEnded /= '1' else '0';

    clock <= TbClock;

    stimuli : process
    begin
        data <= x"0123";

        wait for 2 * FourSegPeriod;

        -- Stop the clock and hence terminate the simulation
        TbSimEnded <= '1';
        wait;
    end process;

end tb;
