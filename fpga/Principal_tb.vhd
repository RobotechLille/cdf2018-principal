-- Testbench automatically generated online
-- at http://vhdl.lapinoo.net
-- Generation date : 25.2.2018 11:52:20 GMT

library ieee;
use ieee.std_logic_1164.all;

entity Principal_tb is
    end Principal_tb;

architecture tb of Principal_tb is

    constant fFpga : INTEGER := 2_000_000;
    constant fBaud : INTEGER := 9600;

    component Principal is
        Generic(
                   fFpga : INTEGER := fFpga;
                   fBaud : INTEGER := fBaud
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
                 BACKECHO: in std_logic
             );
    end component;

    signal CLK          : std_logic;
    signal BTN          : std_logic;
    signal RX           : std_logic;
    signal TX           : std_logic;
    signal LEFTCHA      : std_logic;
    signal LEFTCHB      : std_logic;
    signal RIGHTCHA     : std_logic;
    signal RIGHTCHB     : std_logic;
    signal FRONTTRIGGER : std_logic;
    signal FRONTECHO    : std_logic;
    signal BACKTRIGGER  : std_logic;
    signal BACKECHO     : std_logic;

    constant TbPeriod : time := 500 ns;
    signal TbClock : std_logic := '0';
    signal TbSimEnded : std_logic := '0';

    signal TbDoneWithCapt : std_logic := '0';

    constant BaudPeriod : time := 1E9 ns / fBaud;
    constant CharacterPeriod : time := 10 * BaudPeriod;

    constant CoderPeriod : time := 27611 ns; -- 10 km/h
begin

    dut : Principal
    port map (CLK          => CLK,
              BTN          => BTN,
              RX           => RX,
              TX           => TX,
              LEFTCHA      => LEFTCHA,
              LEFTCHB      => LEFTCHB,
              RIGHTCHA     => RIGHTCHA,
              RIGHTCHB     => RIGHTCHB,
              FRONTTRIGGER => FRONTTRIGGER,
              FRONTECHO    => FRONTECHO,
              BACKTRIGGER  => BACKTRIGGER,
              BACKECHO     => BACKECHO);

    -- Clock generation
    TbClock <= not TbClock after TbPeriod/2 when TbSimEnded /= '1' else '0';

    CLK <= TbClock;

    leftCoder : process
    begin
        while TbSimEnded = '0' loop
            LEFTCHA <= '1';
            wait for CoderPeriod;
            LEFTCHB <= '1';
            wait for CoderPeriod;
            LEFTCHA <= '0';
            wait for CoderPeriod;
            LEFTCHB <= '0';
            wait for CoderPeriod;
        end loop;
        wait;
    end process;

    rightCoder : process
    begin
        while TbSimEnded = '0' loop
            RIGHTCHA <= '0';
            wait for CoderPeriod;
            RIGHTCHB <= '0';
            wait for CoderPeriod;
            RIGHTCHA <= '1';
            wait for CoderPeriod;
            RIGHTCHB <= '1';
            wait for CoderPeriod;
        end loop;
        wait;
    end process;

    frontCapt: process
    begin
        FRONTECHO <= '0';

        wait on FRONTTRIGGER until FRONTTRIGGER = '1';
        wait on FRONTTRIGGER until FRONTTRIGGER = '0';

        wait for 10 ms;
        FRONTECHO <= '1';
        wait for 15 ms;
        FRONTECHO <= '0';
        wait for 35 ms;

        TbDoneWithCapt <= '1';

        wait;
    end process;

    stimuli : process
        procedure send
        (char : std_logic_vector(7 downto 0)) is
        begin
            rx <= '0'; -- Start bit
            wait for BaudPeriod;
            for I in 0 to 7 loop
                rx <= char(I);
                wait for BaudPeriod;
            end loop;
            rx <= '1'; -- Stop bit
            wait for BaudPeriod;
        end procedure;
    begin
        BTN <= '0';
        RX <= '1';
        BACKECHO <= '0';

        -- Reset generation
        BTN <= '1';
        wait for 100 ns;
        BTN <= '0';
        wait for 100 ns;

        wait for 2 * BaudPeriod;


        -- Send 'P'
        send(x"50"); -- 'P'
        wait for CharacterPeriod;

        -- Wait margin
        wait for 2 * BaudPeriod;


        -- Send '?'
        send(x"3F"); -- '?'
        wait for 2 * CharacterPeriod;

        -- Wait margin
        wait for 2 * BaudPeriod;


        -- Send 'D'
        send(x"44");
        wait for 5 * CharacterPeriod;

        wait on TbDoneWithCapt until TbDoneWithCapt = '1';

        -- Send 'C'
        send(x"43");
        wait for 5 * CharacterPeriod;

        -- Wait margin
        wait for 5 * BaudPeriod;

        -- Stop the clock and hence terminate the simulation
        TbSimEnded <= '1';
        wait;
    end process;

end tb;
