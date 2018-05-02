-- Inspiré de http://www.fpga4fun.com/PWM_DAC_2.html

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.ALL;

entity PWM is
    port (
             clk : in std_logic;
             data : in std_logic_vector (7 downto 0) := "00000000";
             pulse : out std_logic
         );
end PWM;

architecture Behavioral of PWM is
    signal  accuI : integer := 0;
    signal  dataI : integer := 0;
begin

    dataI <= to_integer(unsigned(data));

    process(clk, data)
    begin
        if rising_edge(clk) then
            accuI  <=  accuI mod 256 + dataI;
        end if;
    end process;

    pulse <= '1' when accuI > 256 else '0';
end Behavioral;
