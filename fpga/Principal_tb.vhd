-- Testbench automatically generated online
-- at http://vhdl.lapinoo.net
-- Generation date : 25.2.2018 11:52:20 GMT

library ieee;
use ieee.std_logic_1164.all;

entity Principal_tb is
end Principal_tb;

architecture tb of Principal_tb is

    component Principal
        port (CLK    : in std_logic;
              BTN    : in std_logic;
              IO     : inout std_logic_vector (21 downto 20);
              LED    : out std_logic_vector (3 downto 0);
              AN     : out std_logic_vector (3 downto 0);
              A_TO_G : out std_logic_vector (6 downto 0);
              DOT    : out std_logic);
    end component;

    signal CLK    : std_logic;
    signal BTN    : std_logic;
    signal IO     : std_logic_vector (21 downto 20);
    signal LED    : std_logic_vector (3 downto 0);
    signal AN     : std_logic_vector (3 downto 0);
    signal A_TO_G : std_logic_vector (6 downto 0);
    signal DOT    : std_logic;

    constant TbPeriod : time := 20 ns;
    signal TbClock : std_logic := '0';
    signal TbSimEnded : std_logic := '0';

    constant BaudPeriod : time := 104167 ns; -- 9600 baud
    constant CharacterPeriod : time := 10 * BaudPeriod;
    signal rx : std_logic;
    signal tx : std_logic;
begin

    dut : Principal
    port map (CLK    => CLK,
              BTN    => BTN,
              IO     => IO,
              LED    => LED,
              AN     => AN,
              A_TO_G => A_TO_G,
              DOT    => DOT);

    -- Clock generation
    TbClock <= not TbClock after TbPeriod/2 when TbSimEnded /= '1' else '0';

    CLK <= TbClock;

    IO(20) <= rx;
    tx <= IO(21);


    stimuli : process
        variable sending : std_logic_vector(7 downto 0);
    begin
        rx <= '1';

        -- Reset generation
        BTN <= '1';
        wait for 100 ns;
        BTN <= '0';
        wait for 100 ns;

        wait for 2 * BaudPeriod;

        -- Send 'P'
        rx <= '0'; -- Start bit
        sending := x"50"; -- 'P'
        wait for BaudPeriod;
        for I in 0 to 7 loop
            rx <= sending(I);
            wait for BaudPeriod;
        end loop;
        rx <= '1'; -- Stop bit
        wait for BaudPeriod;

        -- Wait for 1 byte receive
        wait for CharacterPeriod;

        -- Wait margin
        wait for 2 * BaudPeriod;

        -- Send '?'
        rx <= '0'; -- Start bit
        sending := x"3F"; -- '?'
        wait for BaudPeriod;
        for I in 0 to 7 loop
            rx <= sending(I);
            wait for BaudPeriod;
        end loop;
        rx <= '1'; -- Stop bit
        wait for BaudPeriod;

        -- Wait for 2 bytes receive
        wait for 2 * CharacterPeriod;

        -- Wait margin
        wait for 2 * BaudPeriod;

        -- Stop the clock and hence terminate the simulation
        TbSimEnded <= '1';
        wait;
    end process;

end tb;
