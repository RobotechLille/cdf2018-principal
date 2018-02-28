-- Code original écrit par Geoffrey Preud'homme et Eloi Zalczer
-- dans le cadre du tutorat de Circuits Numériques Programmables du S7

-- Filtre à réponse impulsionelle finie

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity fir is
    Port (
             clock : in STD_LOGIC; -- Afin de générer l'impulsion terminée
             reset : in STD_LOGIC;
             signalIn : in INTEGER; -- Signal d'entrée (un à la fois)
             signalOut : out INTEGER := 0; -- Signal filtré de sortie
             start : in STD_LOGIC; -- Lance une étape de filtrage
             done : out STD_LOGIC -- Signale la fin de l'étape de filtrage
         );
end fir;

architecture Behavioral of fir is

    constant N : INTEGER := 4; -- Nombre de coefficients
    constant M : INTEGER := 2**6; -- Facteur multiplicatif
    type INT_ARRAY is array (N-1 downto 0) of integer;
    constant coefficients : INT_ARRAY := (16,16,16,16);
    -- ↑ Coefficients du fir multipliés par M

    signal echantillons : INT_ARRAY := (others => 0); -- stockage des entrées retardées
    type fir_states is (waiting, calculating); -- machine à états finis
    signal state : fir_states := waiting;
    signal k : integer := 0;
    signal somme : integer := 0;
begin
    filter: PROCESS(clock)
    begin
        if reset = '1' then
            done <= '0';
            state <= waiting;
            echantillons <= (others => 0);

        elsif rising_edge(clock) then

            if state = waiting then
                done <= '0';

                -- Quand start est à 1, on lance le filtre
                if start = '1' then
                    -- Décalage
                    echantillons(N-1 downto 1) <= echantillons(N-2 downto 0);
                    echantillons(0) <= signalIn;
                    k <= 0;
                    somme <= 0;
                    state <= calculating;
                end if;

            elsif state = calculating then
                -- Calcul de la somme
                somme <= somme + echantillons(k) * coefficients(k);
                k <= k + 1;

                if k = N-1 then
                    -- Division par le facteur
                    signalOut <= somme / M;
                    done <= '1';
                    state <= waiting;
                end if;
            end if;
        end if;
    end process;

end Behavioral;
