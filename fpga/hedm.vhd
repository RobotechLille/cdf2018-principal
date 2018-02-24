-- Process signals from HEDM-550X encoder
-- and output the value read

-- TODO Quelques modifications apportées depuis test avec les vraies codeuses
-- un nouveau test sera nécessaire (vérifier notamment le sens de parcours)

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity hedm is
    Port (
             clk : in STD_LOGIC; -- Horloge, la fréquence n'importe pas
             chA : in STD_LOGIC; -- Canal A
             chB : in STD_LOGIC; -- Canal B
             reset : in STD_LOGIC;
             counts : out integer
         );
end hedm;

architecture Behavioral of hedm is
    signal counter : integer := 0;
    signal An, Bn : STD_LOGIC := '0'; -- Nouvelles valeurs de A et B stockées pour que les entrées soient lues une seule fois en début de cycle
    signal Ap, Bp : STD_LOGIC := '0'; -- Précédentes valeurs de A et B pour détecter les front montant
begin
    processInput : process(clk, reset)
    begin
        if reset = '1' then
            counter <= 0;
            An <= '0';
            Bn <= '0';
            Ap <= '0';
            Bp <= '0';
        elsif rising_edge(clk) then

            Ap <= An;
            Bp <= Bn;

            An <= chA;
            Bn <= chB;

            -- On pourrait optimiser la logique avec un tableau de Karnaugh ou autres méthodes
            -- de simplification d'algèbre de Boole, mais le synthétiseur pour FPGA fera un
            -- tout aussi bon travail, on garde donc le code suivant pour la lisibilité

            if (Ap = '0' and An = '1') then -- Front montant A
                if (Bn = '0') then
                    counter <= counter + 1;
                else
                    counter <= counter - 1;
                end if;
            elsif (Ap = '1' and An = '0') then -- Front descendant A
                if (Bn = '1') then
                    counter <= counter + 1;
                else
                    counter <= counter - 1;
                end if;
            elsif (Bp = '0' and Bn = '1') then -- Front montant B
                if (An = '1') then
                    counter <= counter + 1;
                else
                    counter <= counter - 1;
                end if;
            elsif (Bp = '1' and Bn = '0') then -- Front descendant B
                if (An = '0') then
                    counter <= counter + 1;
                else
                    counter <= counter - 1;
                end if;
            end if;
        end if;
    end process;

    counts <= counter;

end Behavioral;

