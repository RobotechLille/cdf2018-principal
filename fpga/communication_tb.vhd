-- Testbench automatically generated online
-- at http://vhdl.lapinoo.net
-- Generation date : 24.2.2018 16:12:08 GMT

library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;

entity communication_tb is
    end communication_tb;

architecture tb of communication_tb is

    component communication
        port (clock  : in std_logic;
              reset  : in std_logic;
              left   : in integer;
              right  : in integer;
              front  : in integer;
              back   : in integer;
              txData : out std_logic_vector (7 downto 0);
              txStb  : out std_logic;
              txAck  : in std_logic;
              rxData : in std_logic_vector (7 downto 0);
              rxStb  : in std_logic);
    end component;

    signal clock  : std_logic;
    signal reset  : std_logic;
    signal left   : integer;
    signal right  : integer;
    signal front  : integer;
    signal back   : integer;
    signal txData : std_logic_vector (7 downto 0);
    signal txStb  : std_logic;
    signal txAck  : std_logic;
    signal rxData : std_logic_vector (7 downto 0);
    signal rxStb  : std_logic;

    constant TbPeriod : time := 20 ns;
    signal TbClock : std_logic := '0';
    signal TbSimEnded : std_logic := '0';

    type multipleChar is array (0 to 15) of std_logic_vector(7 downto 0);

begin

    dut : communication
    port map (clock  => clock,
              reset  => reset,
              left   => left,
              right  => right,
              front  => front,
              back   => back,
              txData => txData,
              txStb  => txStb,
              txAck  => txAck,
              rxData => rxData,
              rxStb  => rxStb);

    -- Clock generation
    TbClock <= not TbClock after TbPeriod/2 when TbSimEnded /= '1' else '0';

    clock <= TbClock;

    stimuli : process
        variable shouldReceive : multipleChar;
    begin
        left <= 0;
        right <= 0;
        front <= 0;
        back <= 0;
        txAck <= '0';
        rxData <= (others => '0');
        rxStb <= '0';

        -- Reset generation
        reset <= '1';
        wait for 100 ns;
        reset <= '0';
        wait for 100 ns;


        -- Test Ping
        report "TEST Receiving 'P'" severity note;
        rxData <= x"50";
        rxStb <= '1';
        wait for TbPeriod;
        rxStb <= '0';

        wait for 100 ns;
        assert txData = x"50" report "Not sent 'P'" severity error;
        assert txStb = '1' report "Not sending" severity error;

        report "Acknowledging send" severity note;
        wait for 100 ns;
        txAck <= '1';
        wait for TbPeriod;
        txAck <= '0';

        wait for 100 ns;
        assert txStb = '0' report "Not stopping send" severity error;

        wait for 100 ns; -- Margin


        -- Test unknown char
        report "TEST Receiving '?'" severity note;
        rxData <= x"3F";
        rxStb <= '1';
        wait for TbPeriod;
        rxStb <= '0';

        wait for 100 ns;
        assert txData = x"45" report "Not sent 'E'" severity error;
        assert txStb = '1' report "Not sending" severity error;

        report "Acknowledging send" severity note;
        wait for 100 ns;
        txAck <= '1';
        wait for TbPeriod;
        txAck <= '0';

        wait for 100 ns;
        assert txData = x"43" report "Not sent 'C'" severity error;
        assert txStb = '1' report "Not sending" severity error;

        report "Acknowledging send" severity note;
        wait for 100 ns;
        txAck <= '1';
        wait for TbPeriod;
        txAck <= '0';

        wait for 100 ns;
        assert txStb = '0' report "Not stopping send" severity error;
        wait for 100 ns; -- Margin

        -- Test queue
        report "TEST Receiving 'P' three times" severity note;
        rxData <= x"50";
        for I in 0 to 2 loop
            rxStb <= '1';
            wait for TbPeriod;
            rxStb <= '0';
            wait for 100 ns;
        end loop;

        for I in 0 to 2 loop
            wait for 100 ns;
            assert txData = x"50" report "Not sent 'P'" severity error;
            assert txStb = '1' report "Not sending" severity error;

            report "Acknowledging send" severity note;
            wait for 100 ns;
            txAck <= '1';
            wait for TbPeriod;
            txAck <= '0';
        end loop;

        wait for 100 ns;
        assert txStb = '0' report "Not stopping send" severity error;

        -- Test captor
        front <= 1152;
        back <= 11614;

        report "TEST Receiving 'C'" severity note;
        rxData <= x"43";
        rxStb <= '1';
        wait for TbPeriod;
        rxStb <= '0';

        shouldReceive(0 to 4) := (x"43", x"80", x"04", x"5E", x"2D");
        for I in 0 to 4 loop
            wait for 100 ns;
            assert txData = shouldReceive(I) report "Not sent correct data, got " & integer'image(to_integer(unsigned(txData))) & ", expected " & integer'image(to_integer(unsigned(shouldReceive(I))))severity error;
            assert txStb = '1' report "Not sending" severity error;

            report "Acknowledging send" severity note;
            wait for 100 ns;
            txAck <= '1';
            wait for TbPeriod;
            txAck <= '0';
        end loop;


        wait for 100 ns; -- Margin

        TbSimEnded <= '1';
        wait;
    end process;

end tb;
