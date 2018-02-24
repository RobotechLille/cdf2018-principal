-- Testbench automatically generated online
-- at http://vhdl.lapinoo.net
-- Generation date : 24.2.2018 16:12:08 GMT

library ieee;
use ieee.std_logic_1164.all;

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
        report "Receiving 'P'" severity note;
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
        report "Receiving '?'" severity note;
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


        TbSimEnded <= '1';
        wait;
    end process;

end tb;
