LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_arith.all;

ENTITY hcSr04 IS
    GENERIC(
               fFpga : INTEGER := 50_000_000; -- frequency of the FPGA clock (Hz)
               maxOutput : INTEGER := 65535 -- maximum number to store the distance
           );
PORT(
        clk : IN STD_LOGIC; -- clock of the FPGA
        echo : IN STD_LOGIC; -- echo pin of the hcSr04
        distance : OUT INTEGER RANGE 0 TO maxOutput; -- Ranges from 0 to 4 meters, 0 if no data
        trigger : OUT STD_LOGIC; -- trigger pin of the hcSr04
        start : IN STD_LOGIC; -- Set to '1' everytime a measurement is needed (or keep at '1' for continuous measurement)
        finished : OUT STD_LOGIC -- Driven to '1' everytime a measurement has finished
        );
END hcSr04;

ARCHITECTURE Behavioral OF hcSr04 IS
    CONSTANT triggerTicks : INTEGER := fFPGA / 100000; -- Number of FPGA ticks that makes 10us (used for trigger)
    CONSTANT measurementTicks : INTEGER := fFPGA / 17; -- Number of FPGA ticks that makes 60ms (used for measurement cycles)
    CONSTANT maximumRange : INTEGER := 4; -- Maximum range the sensor can detect
    CONSTANT distanceTicks : INTEGER := maximumRange * fFPGA / 172; -- Number of FPGA ticks that makes the maximum distance that can be measured
    -- 172 = 1 / 58 s/m
    SIGNAL measurementCounter : INTEGER RANGE 0 to measurementTicks - 1 := 0; -- Progress in the measurement
    SIGNAL distanceCounter : INTEGER RANGE 0 to distanceTicks - 1 := 0; -- Ticks for wich echo has been at one
    SIGNAL triggerCounter : INTEGER RANGE 0 to triggerTicks - 1 := 0; -- Progress in the trigger
    TYPE stateType IS (init, waiting, triggering, measuring);
    SIGNAL state : stateType;
BEGIN
    measure : process(clk)
    begin
        if rising_edge(clk) then
            CASE state IS
                WHEN init =>
                    trigger <= '0';
                    distance <= 0;
                    state <= waiting;
                WHEN waiting =>
                    finished <= '0';
                    IF start = '1' THEN
                        trigger <= '1';
                        triggerCounter <= 0;
                        state <= triggering;
                    END IF;
                WHEN triggering =>
                    triggerCounter <= triggerCounter + 1;
                    IF triggerCounter = triggerTicks - 1 THEN
                        trigger <= '0';
                        measurementCounter <= 0;
                        distanceCounter <= 0;
                        state <= measuring;
                    END IF;
                WHEN measuring =>
                    IF echo = '1' and distanceCounter < distanceTicks THEN
                        distanceCounter <= distanceCounter + 1;
                    END IF;
                    measurementCounter <= measurementCounter + 1;
                    IF measurementCounter = measurementTicks - 1 THEN
                        distance <= distanceCounter * maxOutput / distanceTicks;
                        finished <= '1';
                        state <= waiting;
                    END IF;
            END CASE;
        end if;
    end process;
END Behavioral;
