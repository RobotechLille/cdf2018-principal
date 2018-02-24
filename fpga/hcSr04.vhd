LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY hcSr04 IS
    GENERIC(
               fFpga : INTEGER := 50_000_000 -- frequency of the FPGA clock (Hz)
           );
PORT(
        clk : IN STD_LOGIC; -- clock of the FPGA
        echo : IN STD_LOGIC; -- echo pin of the hcSr04
        distance : OUT INTEGER RANGE 0 TO 65535; -- Divide by 58 to get the value in cm
        trigger : OUT STD_LOGIC; -- trigger pin of the hcSr04
        start : IN STD_LOGIC; -- Set to '1' everytime a measurement is needed (or keep at '1' for continuous measurement)
        finished : OUT STD_LOGIC -- Driven to '1' everytime a measurement has finished
        );
END hcSr04;

ARCHITECTURE Behavioral OF hcSr04 IS
    -- Generate us clock
    CONSTANT fUs : INTEGER := 1_000_000; -- Frequency of the microsecond clock
    CONSTANT usTicks : INTEGER := fFPGA / fUs; -- Number of FPGA tick that makes a microsecond
    SIGNAL fpgaCounter : INTEGER RANGE 0 TO usTicks - 1; -- Count the microsecond
    SIGNAL usClk : STD_LOGIC; -- Clock that ticks every us

    -- Trigger
    CONSTANT triggerDuration : INTEGER := 10; -- Number of us that makes up a trigger sequence
    SIGNAL triggerCounter : INTEGER RANGE 0 TO triggerDuration - 1 := 0; -- Progress in the trigger sequence
    SIGNAL theTrigger : STD_LOGIC := '0'; -- Trigger pin but with default value

    -- Measurement
    CONSTANT measurementDuration : INTEGER := 60_000; -- Number of us that makes up a measurement cycle
    SIGNAL measurementCounter : INTEGER RANGE 0 TO measurementDuration - 1 := 0; -- Progress in the measurement cycle

    -- Distance
    SIGNAL distanceCounter : INTEGER RANGE 0 TO 65535 := 0; -- Distance measured (in us)

    -- State machine
    TYPE stateType IS (waiting, triggering, measuring);
    SIGNAL state : stateType;

BEGIN
    us : process(clk)
    begin
        if rising_edge(clk) then
            if fpgaCounter = 0 then
                usClk <= '0';
            elsif fpgaCounter = 1 then
                usClk <= '1';
            end if;
            fpgaCounter <= fpgaCounter + 1;
        end if;
    end process;

    trigger <= theTrigger;

    measure : process(usClk)
    begin
        if rising_edge(usClk) then
            CASE state IS
                WHEN waiting =>
                    finished <= '0';
                    IF start = '1' THEN
                        theTrigger <= '1';
                        triggerCounter <= 0;
                        state <= triggering;
                    END IF;
                WHEN triggering =>
                    triggerCounter <= triggerCounter + 1;
                    IF triggerCounter = triggerDuration - 1 THEN
                        theTrigger <= '0';
                        measurementCounter <= 0;
                        distanceCounter <= 0;
                        state <= measuring;
                    END IF;
                WHEN measuring =>
                    IF echo = '1' and distanceCounter < 65535 THEN
                        distanceCounter <= distanceCounter + 1;
                    END IF;
                    measurementCounter <= measurementCounter + 1;
                    IF measurementCounter = measurementDuration - 1 THEN
                        distance <= distanceCounter;
                        finished <= '1';
                        state <= waiting;
                    END IF;
            END CASE;
        end if;
    end process;
END Behavioral;
