library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity sevenseg is
    Port (
             data : in  STD_LOGIC_VECTOR (15 downto 0);
             clock : in STD_LOGIC;
             anode : out  STD_LOGIC_VECTOR (3 downto 0);
             segment : out  STD_LOGIC_VECTOR (6 downto 0);
             dot : out  STD_LOGIC
         );
end sevenseg;

architecture structural of sevenseg is

    signal digit: std_logic_vector(3 downto 0);
    signal selector: integer := 0;
    signal counter : integer := 0;

begin
    with selector select
        anode <= "1110" when 0,
                 "1101" when 1,
                 "1011" when 2,
                 "0111" when 3,
                 "0000" when others;

    with selector select
        digit <= data(3 downto 0)   when 0,
                 data(7 downto 4)   when 1,
                 data(11 downto 8)  when 2,
                 data(15 downto 12) when 3,
                 "0000"             when others;

    with digit select
        segment <= "0000001" when "0000", -- 0
                   "1001111" when "0001", -- 1
                   "0010010" when "0010", -- 2
                   "0000110" when "0011", -- 3
                   "1001100" when "0100", -- 4
                   "0100100" when "0101", -- 5
                   "0100000" when "0110", -- 6
                   "0001111" when "0111", -- 7
                   "0000000" when "1000", -- 8
                   "0000100" when "1001", -- 9
                   "0001000" when "1010", -- A
                   "1100000" when "1011", -- b
                   "0110001" when "1100", -- C
                   "1000010" when "1101", -- d
                   "0110000" when "1110", -- E
                   "0111000" when "1111", -- F
                   "0000000" when others;

    dot <= '1';

    alternateur : process(clock)
    begin
        if clock'event and clock = '1' then
            counter <= counter + 1;
            if counter >= 199999 then
                selector <= selector + 1;
                if selector >= 3 then
                    selector <= 0;
                end if;
                counter <= 0;
            end if;
        end if;
    end process;


end structural;

