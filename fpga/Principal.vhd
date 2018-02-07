library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity Principal is
    Port ( CLK : in  STD_LOGIC;
           LED : out  STD_LOGIC_VECTOR (3 downto 0));
end Principal;

architecture Behavioral of Principal is
    signal pulse : std_logic := '0';
    signal count : integer range 0 to 49999999 := 0;
begin

    counter : process(CLK)
    begin
        if CLK'event and CLK = '1' then
            if count = 49999999 then
                count <= 0;
                pulse <= not pulse;
            else
                count <= count + 1;
            end if;
        end if;
    end process;

    LED(3 downto 0) <= (others => pulse);
end Behavioral;

