-- Testbench automatically generated online
-- at http://vhdl.lapinoo.net
-- Generation date : 24.2.2018 15:15:56 GMT

library ieee;
use ieee.std_logic_1164.all;

entity hedm_tb is
    end hedm_tb;

architecture tb of hedm_tb is

    component hedm
        port (clk    : in std_logic;
              chA    : in std_logic;
              chB    : in std_logic;
              reset  : in std_logic;
              zero   : in std_logic;
              counts : out integer);
    end component;

    signal clk    : std_logic;
    signal chA    : std_logic;
    signal chB    : std_logic;
    signal reset  : std_logic;
    signal zero   : std_logic;
    signal counts : integer;

    constant TbPeriod : time := 20 ns;
    signal TbClock : std_logic := '0';
    signal TbSimEnded : std_logic := '0';

begin

    dut : hedm
    port map (clk    => clk,
              chA    => chA,
              chB    => chB,
              reset  => reset,
              zero  => zero,
              counts => counts);

    -- Clock generation
    TbClock <= not TbClock after TbPeriod/2 when TbSimEnded /= '1' else '0';

    clk <= TbClock;

    stimuli : process
        variable tour : integer;
        constant nbTours : integer := 10;
    begin
        chA <= '0';
        chB <= '0';
        zero <= '0';

        -- Reset generation
        reset <= '1';
        wait for 100 ns;
        reset <= '0';
        wait for 100 ns;

        -- Test sens avant
        for I in 0 to nbTours-1 loop
            chA <= '1';
            wait for TbPeriod;
            chB <= '1';
            wait for TbPeriod;
            chA <= '0';
            wait for TbPeriod;
            chB <= '0';
            wait for TbPeriod;
        end loop;

        wait for 5 * TbPeriod;
        assert counts = nbTours * 4 report "Sens avant faux, reçu " & integer'image(counts) severity error;

        -- Test zero
        zero <= '1';
        wait for TbPeriod;
        zero <= '0';
        wait for TbPeriod;

        wait for 5 * TbPeriod;
        assert counts = 0 report "Zero faux, reçu " & integer'image(counts) severity error;


        -- Test sens arrière
        for I in 0 to nbTours-1 loop
            chB <= '1';
            wait for TbPeriod;
            chA <= '1';
            wait for TbPeriod;
            chB <= '0';
            wait for TbPeriod;
            chA <= '0';
            wait for TbPeriod;
        end loop;

        wait for 5 * TbPeriod;
        assert counts = -40 report "Sens arrière faux, reçu " & integer'image(counts) severity error;

        -- Test zero en éxecution
        chA <= '1';
        wait for TbPeriod;
        chB <= '1';
        wait for TbPeriod;
        chA <= '0';
        zero <= '1';
        wait for TbPeriod;
        chB <= '0';
        zero <= '0';
        wait for TbPeriod;

        wait for 5 * TbPeriod;
        assert counts = 3 report "Zero en éxecution faux, reçu " & integer'image(counts) severity error;

        zero <= '1';
        wait for TbPeriod;
        zero <= '0';
        wait for TbPeriod;

        -- Test aller-retours
        for I in 0 to nbTours-1 loop
            chA <= '1';
            wait for TbPeriod;
            chB <= '1';
            wait for TbPeriod;
            chB <= '0';
            wait for TbPeriod;
            chA <= '0';
            wait for TbPeriod;
        end loop;

        wait for 5 * TbPeriod;
        assert counts = 0 report "Aller-retours faux, reçu " & integer'image(counts) severity error;



        -- Stop the clock and hence terminate the simulation
        TbSimEnded <= '1';
        wait;
    end process;

end tb;
