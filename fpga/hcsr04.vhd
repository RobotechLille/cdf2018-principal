LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY hcsr04 IS
    GENERIC(
               fFpga : INTEGER := 50_000_000 -- frequency of the FPGA clock (Hz)
           );
    PORT(
            clk : IN STD_LOGIC; -- clock of the FPGA
            reset : IN STD_LOGIC; -- hard reset
            echo : IN STD_LOGIC; -- echo pin of the hcsr04
            distance : OUT INTEGER; -- Divide by 58 to get the value in cm
            trigger : OUT STD_LOGIC; -- trigger pin of the hcsr04
            start : IN STD_LOGIC; -- Set to '1' everytime a measurement is needed (or keep at '1' for continuous measurement)
            finished : OUT STD_LOGIC -- Driven to '1' everytime a measurement has finished
        );
END hcsr04;

ARCHITECTURE Behavioral OF hcsr04 IS
    -- Generate us clock
    CONSTANT fUs : INTEGER := 1_000_000; -- Frequency of the microsecond clock
    CONSTANT usTicks : INTEGER := fFPGA / fUs / 2; -- Number of FPGA tick that makes a demi-microsecond
    SIGNAL fpgaCounter : INTEGER; -- Count the microsecond
    SIGNAL usClk : STD_LOGIC := '0'; -- Clock that ticks every us

    -- Trigger
    CONSTANT triggerDuration : INTEGER := 10; -- Number of us that makes up a trigger sequence
    SIGNAL triggerCounter : INTEGER; -- Progress in the trigger sequence
    SIGNAL theTrigger : STD_LOGIC := '0'; -- Trigger pin but with default value

    -- Measurement
    CONSTANT measurementDuration : INTEGER := 60_000; -- Number of us that makes up a measurement cycle
    SIGNAL measurementCounter : INTEGER := 0; -- Progress in the measurement cycle

    -- Distance
    CONSTANT maxDistance : INTEGER := 29000; -- 5m
    SIGNAL distanceCounter : INTEGER := 0; -- Distance measured (in us)

    -- State machine
    TYPE stateType IS (waiting, triggering, measuring);
    SIGNAL state : stateType;
    SIGNAL startUs : STD_LOGIC := '0'; -- A propagation of the start signal

BEGIN
    us : process(clk, reset)
    begin
        if reset = '1' then
            usClk <= '0';
            fpgaCounter <= 0;
            startUs <= '0';
        elsif rising_edge(clk) then

            if start = '1' then
                startUs <= '1';
            elsif state /= waiting then
                startUs <= '0';
            end if;

            if fpgaCounter >= usTicks - 1 then
                fpgaCounter <= 0;
                usClk <= not usClk;
            else
                fpgaCounter <= fpgaCounter + 1;
            end if;
        end if;
    end process;

    trigger <= theTrigger;

    measure : process(usClk, reset)
    begin
        if reset = '1' then
            state <= waiting;
            finished <= '0';
            distance <= 0;
        elsif rising_edge(usClk) then
            CASE state IS
                WHEN waiting =>
                    finished <= '0';
                    IF startUs = '1' THEN
                        theTrigger <= '1';
                        triggerCounter <= 0;
                        state <= triggering;
                    END IF;
                WHEN triggering =>
                    triggerCounter <= triggerCounter + 1;
                    IF triggerCounter >= triggerDuration - 1 THEN
                        theTrigger <= '0';
                        measurementCounter <= 0;
                        distanceCounter <= 0;
                        state <= measuring;
                    END IF;
                WHEN measuring =>
                    IF echo = '1' and distanceCounter < maxDistance THEN
                        distanceCounter <= distanceCounter + 1;
                    END IF;
                    measurementCounter <= measurementCounter + 1;
                    IF measurementCounter >= measurementDuration - 1 THEN
                        distance <= distanceCounter;
                        finished <= '1';
                        state <= waiting;
                    END IF;
            END CASE;
        end if;
    end process;
END Behavioral;
