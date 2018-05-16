global SIMULATION;
SIMULATION = 0;

% Paramètres de lecture
DIRNAME = "/home/geoffrey/CdF/cdf2018-principal/log/";
FILENAME = "001418.csv";
PATH = DIRNAME + FILENAME;

% Paramètres de simulation
global SIMULATION_TIME SIMULATION_DT;
SIMULATION_TIME = 10;
SIMULATION_DT = 1e-15;

%BEGIN DIMENSIONS

% Dimensions pistes
global pisteWidth pisteHeight pisteOrigX pisteOrigY;
pisteWidth = 3000.0;
pisteHeight = 2000.0;
pisteOrigX = 0.0;
pisteOrigY = 0.0;

% Dimensions robot
global width height distanceBetweenWheels wheelDiameter wheelPerimeter motorSpeedGainRPMpV motorSpeedGain motorNominalTension motorControllerAlimentation motorControllerReference motorSaturationMin motorSaturationMax pwmMax coderResolution coderDataFactor coderDataResolution cranReducOut cranReducIn reducRatio coderFullResolution avPerCycle;
width = 250.0; % mm (from meca)
height = 100.0; % mm (from random)
distanceBetweenWheels = width; % mm (from meca)
wheelDiameter = 80.0; % mm (from meca)
wheelPerimeter = (wheelDiameter * pi); % mm
motorSpeedGainRPMpV = 233.0; % rpm/V (from datasheet)
motorSpeedGain = (motorSpeedGainRPMpV / 60.0); % motor rev/s/V
motorNominalTension = 24.0; % V (from datasheet)
motorControllerAlimentation = 24.0; % V (from elec)
motorControllerReference = 5.0; % V (from wiring)
motorSaturationMin = 0.0; % V (from random)
motorSaturationMax = 4.0; % V (from testing)
pwmMax = 3.3; % V (from FPGA datasheet)
coderResolution = 370.0; % cycles/motor rev
coderDataFactor = 4.0; % increments/motor cycles
coderDataResolution = (coderResolution * coderDataFactor); % cycles/motor rev
cranReducOut = 48.0; % nb crans (from meca)
cranReducIn = 12.0; % nb crans (from meca)
reducRatio = (cranReducIn / cranReducOut); % reduction ratio
coderFullResolution = (coderDataResolution / reducRatio); % cycles / wheel rev
avPerCycle = (wheelPerimeter / coderFullResolution); % mm

% Pour éviter les pics de codeuse liées à la communication I2C
absoluteMaxVitesseRobot = 10.0; % km/h
absoluteMaxVitesseRobotMMpS = (absoluteMaxVitesseRobot * 10000.0 / 36.0); % mm/s
absoluteMaxVitesseRobotRevpS = (absoluteMaxVitesseRobotMMpS / wheelPerimeter); % rev/s
absoluteMaxVitesseRobotCycpS = (absoluteMaxVitesseRobotRevpS * coderFullResolution); % cycle/s

% Constantes asservissement
global oTensionMin dTensionMin oVitMin dVitMin dDirEcartMin dDirEcartMax oDirEcartMin oDirEcartMax oEcartMin oEcartMax targetTensionRatio targetTension carotteDistance carotteAngle dKP dKI dKD oKP oKI oKD margeSecurite;

% Asservissement en angle
oVitMin = 0.5; % rad/s
oTensionMin = 1; % V
oDirEcartMin = (20.0 / 360.0 * 2.0 * pi); % rad
oEcartMin = (10.0 / 360.0 * 2.0 * pi); % rad
oEcartMax = (20.0 / 360.0 * 2.0 * pi); % rad
derivPi = (motorSaturationMax / (wheelPerimeter * pi));
oKP = (3.0 * derivPi); % au max peut dérivier de pi
oKI = 0.0;
oKD = 0.0;
carotteAngle = (targetTension / oKP); % mm

% Asservissement en distance
dVitMin = 10.0; % mm/s
dTensionMin = 1; % V
dDirEcartMin = 20.0; % mm
dDirEcartMax = 70.0; % mm
dKP = 0.1;
dKI = 0.0;
dKD = 0.0;
targetTensionRatio = 0.75;
targetTension = (targetTensionRatio * motorSaturationMax); % V
carotteDistance = (targetTension / dKP); % mm

margeSecurite = 300.0; % mm

%END DIMENSIONS


global s;
if SIMULATION == 1
    % Génération de la consigne
    xinit = 50;
    yinit = 50;
    oinit = 4 * pi;
    d1t = 2;
    d1x = -300;
    d1y = 0;
    d1o = 2 * pi;
    dt = SIMULATION_DT;

    global xcons ycons ocons;
    xcons = timeseries([xinit, xinit, d1x, d1x], [0 d1t-dt d1t SIMULATION_TIME]);
    ycons = timeseries([yinit, yinit, d1y, d1y], [0 d1t-dt d1t SIMULATION_TIME]);
    ocons = timeseries([oinit, oinit, d1o, d1o], [0 d1t-dt d1t SIMULATION_TIME]);

    % Simulation
    disp("Lancement de la simulation");
    s = sim("modelisation", "StopTime", string(SIMULATION_TIME), "MinStep", string(SIMULATION_DT));
    fprintf("Simulation sampling rate: %f Hz\n", length(s.tout)/SIMULATION_TIME);
else
    disp("Ouverture des données");
    T = readtable(PATH);
    
    % Données pratiques → données théoriques
    T.time(1) = 0;
    T.x = T.xConnu;
    T.y = T.yConnu;
    T.o = T.oConnu;
    
    T.secBackL = -T.secBackL;
    T.secBackR = -T.secBackR;

    disp("Enregistrement des données");
    s = containers.Map;
    for name=T.Properties.VariableNames
        nameChar = char(name);
        s(nameChar) = timeseries(T.(nameChar), T.time);
    end
    
    % Modification pour faire passer comme une simu
    td = getTimePoints();
    SIMULATION_TIME = td(end);

end


% Graphes

global curGraph graphWidth graphHeight;
curGraph = 1;
graphWidth = 3;
graphHeight = 2;

clf
graphSpatiale();
graphRoues();
graphCodeuses();
graphDistance();
graphRotation();
graphSecurite();
%graphVitesseDist();
%graphVitesseAngl();
%graphEtat();

updateToTime(SIMULATION_DT);

% FONCTIONS

% Données

function ts = getTS(name)
    global SIMULATION s;
    if SIMULATION == 1
        ts = s.(name);
    else
        name = char(name);
        if s.isKey(name)
            ts = s(name);
        else
            fprintf("Données inconnues : %s\n", name);
            ts = timeseries();
        end
    end
end

function pt = getTimePoints()
    global SIMULATION s;
    if SIMULATION == 1
        pt = s.tout;
    else
        ts = getTS('time');
        pt = ts.Time;
    end
end

function d = getTSData(name, i)
    ts = getTS(name);
    if isempty(ts.Data)
        d = 0;
    else
        d = ts.Data(i);
    end
end

% Dessin

function [x, y] = pointArround(xC, yC, xD, yD, o)
    D = xD + yD * 1i;
    F = abs(D) .* exp(1i * (angle(D) + o - pi/2));
    x = xC + real(F);
    y = yC + imag(F);
end

function drawRect(p, x, y, o, w, h)
    [x1, y1] = pointArround(x, y, - w/2, + h/2, o);
    [x2, y2] = pointArround(x, y, + w/2, + h/2, o);
    [x3, y3] = pointArround(x, y, + w/2, - h/2, o);
    [x4, y4] = pointArround(x, y, - w/2, - h/2, o);
    p.XData = [x1, x2, x3, x4, x1];
    p.YData = [y1, y2, y3, y4, y1];
end

function timeGraph(series)
    global SIMULATION_TIME p;
    m = inf;
    M = -inf;
    for sname=series
        serie = getTS(sname);
        plot(serie);
        if ~isempty(serie.Data)
            m = min(m, min(serie));
        end
        if ~isempty(serie.Data)
            M = max(M, max(serie));
        end
    end
    xlim([0 SIMULATION_TIME]);
    if (abs(m) ~= inf) && (abs(M) ~= inf)
        ylim([m M+1E-9]);
    end
    addTimeline(p);
end

function addLimitline(p, x)
    line(p.XLim, [x x], 'Color', [0.8 0.8 0.8], 'LineStyle', '--');
end

function addTimeline(p)
    global t timelines;
    timeline = line([t t], p.YLim, 'Color', [0 0 0]);
    timelines = [timelines timeline];
end

% Graphiques

function p = newGraph()
    global curGraph graphWidth graphHeight p;
    fprintf("Graphe %d/%d\n", curGraph, graphWidth * graphHeight);
    p = subplot(graphHeight, graphWidth, curGraph);
    hold on;
    curGraph = curGraph + 1;
end

function graphSpatiale()
    p = newGraph();
    global SIMULATION_TIME;
    global t speed playing timelines;
    t = 0;
    speed = 1;
    playing = 0;
    timelines = [];
    global timeSlider;
    timeSlider = uicontrol('Style', 'slider', 'Callback', @sliderCallback, 'Min', 0, 'Max', SIMULATION_TIME, 'Position', [20 20 500 20], 'Value', t);
    global timeText;
    timeText = uicontrol('Style', 'text', 'Position', [520 20 120 20], 'String', sprintf("t = %f", t));
    uicontrol('Style', 'pushbutton', 'String', 'Play', 'Position', [640 20 60 20], 'Callback', @playCallback);
    uicontrol('Style', 'pushbutton', 'String', 'Pause', 'Position', [700 20 60 20], 'Callback', @pauseCallback);
    
    hold on;
    xTs = getTS('x');
    yTs = getTS('y');
    plot(xTs.Data, yTs.Data, 'b--');
    global height;
    global lQuiver;
    lQuiver = quiver(0, 0, 0, 0, 'Color', 'Red', 'MaxHeadSize', height/4);
    global rQuiver;
    rQuiver = quiver(0, 0, 0, 0, 'Color', 'Red', 'MaxHeadSize', height/4);
    global robotRect;
    robotRect = plot(0, 0);
    global robotPath;
    robotPath = plot(0, 0, 'b');
    global dirQuiver;
    dirQuiver = quiver(0, 0, 0, 0, 'Color', 'Red', 'MaxHeadSize', height/2);
    global consQuiver;
    consQuiver = quiver(0, 0, 0, 0, 'Color', 'Green', 'MaxHeadSize', height/2);
    
    % Draw track
    global pisteWidth;
    global pisteHeight;
    global pisteOrigX;
    global pisteOrigY;
    rectangle('Position', [pisteOrigX pisteOrigY pisteWidth pisteHeight]);
    
    % Set limits
    margin = 300;
    ampl = max(xTs.max - xTs.min, yTs.max - yTs.min);
    xlim([xTs.min - margin, xTs.min + ampl + margin]);
    ylim([yTs.min - margin, yTs.min + ampl + margin]);
    title("Évolution spatiale");
    xlabel("X (mm)");
    ylabel("Y (mm)");
end

function graphRoues()
    p = newGraph();
    timeGraph(["lVolt", "rVolt", "dVolt", "oVolt"]);
    global motorSaturationMin motorSaturationMax;
    addLimitline(p, -motorSaturationMin);
    addLimitline(p, -motorSaturationMax);
    addLimitline(p, motorSaturationMin);
    addLimitline(p, motorSaturationMax);
    addLimitline(p, 0);
    title("Roues");
    xlabel("Temps (s)");
    ylabel("Tension (V)");
    legend("Tension gauche", "Tension droite", "Dont distance", "Dont direction");
end

function graphCodeuses()
    p = newGraph();
    timeGraph(["lCodTot", "rCodTot", "newL", "newR"]);
    addLimitline(p, 2^16-1);
    addLimitline(p, 0);
    title("Codeuses");
    xlabel("Temps (s)");
    ylabel("Crans");
    legend("Total gauche", "Total droite", "Brut gauche", "Brut droite");
end

function graphDistance()
    p = newGraph();
    timeGraph(["dDirEcart", "dEcart", "oErr"]);
    global dDirEcartMin dDirEcartMax;
    addLimitline(p, 0);
    addLimitline(p, dDirEcartMin);
    addLimitline(p, dDirEcartMax);
    addLimitline(p, -dDirEcartMin);
    addLimitline(p, -dDirEcartMax);
    title("Distance");
    xlabel("Temps (s)");
    ylabel("Distance (mm)");
    legend("Err. distance", "Err. retenue", "Err rotation");
end

function graphRotation()
    p = newGraph();
    timeGraph(["oDirEcart", "oConsEcart", "oEcart"]);
    global oDirEcartMin;
    addLimitline(p, oDirEcartMin);
    addLimitline(p, -oDirEcartMin);
    title("Rotation");
    xlabel("Temps (s)");
    ylabel("Angle (rad)");
    legend("Err. direction", "Err. consigne", "Err. retenue");
end

function graphSecurite()
    p = newGraph();
    timeGraph(["dDirEcart", "secFrontL", "secFrontR", "secBackL", "secBackR", "dErr"]);
    global margeSecurite;
    addLimitline(p, 0);
    addLimitline(p, margeSecurite);
    addLimitline(p, -margeSecurite);
    title("Distances de sécurité");
    xlabel("Temps (s)");
    ylabel("Distance (mm)");
    legend("Err. distance", "Avant gauche", "Avant droite", "Arrière gauche", "Arrière droite", "Err. retenue");
end

function graphVitesseDist()
    p = newGraph();
    timeGraph(["xVit", "yVit"]);
    addLimitline(p, 0);
    title("Vitesse");
    xlabel("Temps (s)");
    ylabel("Vitesse (mm/s)");
    legend("X", "Y");
end

function graphEtat()
    p = newGraph();
    timeGraph(["etat"]);
    addLimitline(p, 0);
    title("Etat");
    xlabel("Temps (s)");
    %ylabel("Vitesse (mm/s)");
    %legend("X", "Y");
end

function graphVitesseAngl()
    p = newGraph();
    timeGraph(["oVit"]);
    addLimitline(p, 0);
    title("Vitesse");
    xlabel("Temps (s)");
    ylabel("Vitesse (rad/s)");
    legend("O");
end

% Playback

function play()
    global SIMULATION_TIME speed t playing;
    if playing == 1
        return
    end
    startCpu=cputime;
    startT=t;
    n=0;
    playing=1;
    while t<SIMULATION_TIME && playing == 1
        updateToTime((cputime-startCpu)*speed + startT);
        drawnow limitrate;
        n = n + 1;
    end
    playing=0;
    fprintf("Refresh rate : %f Hz\n", n/(cputime-startCpu));
end

function sliderCallback(hObject, ~)
    updateToTime(get(hObject, 'Value'));
end

function playCallback(~, ~)
    play();
end

function pauseCallback(~, ~)
    global playing;
    playing=0;
end

function updateToTime(newT)
    % Update ui
    global timeSlider timeText t SIMULATION_TIME;
    t = max(min(SIMULATION_TIME, newT), 0);
    timeSlider.Value = t;
    timeText.String = sprintf("t = %f", t);

    % Get values
    i = find(getTimePoints() <= t);
    i = i(end);
    x = getTSData('x', i);
    y = getTSData('y', i);
    o = getTSData('o', i);
    xCons = getTSData('xCons', i);
    yCons = getTSData('yCons', i);
    oCons = getTSData('oCons', i);
    lVit = getTSData('lVit', i);
    rVit = getTSData('rVit', i);
    
    
    % Add event
    tEvent = tsdata.event('tEvent', t);
    xTs = getTS('x');
    xS = xTs.addevent(tEvent);
    yTs = getTS('y');
    yS = yTs.addevent(tEvent);
    
    % Draw path
    global robotPath;
    robotPath.XData = xS.gettsbeforeevent('tEvent').Data;
    robotPath.YData = yS.gettsbeforeevent('tEvent').Data;
    
    % Draw robot
    global width height;
    global robotRect dirQuiver;
    drawRect(robotRect, x, y, o, width, height);
    dirQuiver.XData = x; dirQuiver.YData = y;
    dirQuiver.UData = cos(o) * height/2; dirQuiver.VData = sin(o) * height/2;
    % Arrow for wheels
    global lQuiver rQuiver;
    [lQuiver.XData, lQuiver.YData] = pointArround(x, y, -width/2, 0, o);
    lQuiver.UData = cos(o) * lVit; lQuiver.VData = sin(o) * lVit;
    [rQuiver.XData, rQuiver.YData] = pointArround(x, y, +width/2, 0, o);
    rQuiver.UData = cos(o) * rVit; rQuiver.VData = sin(o) * rVit;

    % Draw cons
    global consQuiver;
    consQuiver.XData = xCons; consQuiver.YData = yCons;
    consQuiver.UData = cos(oCons) * height/2 ; consQuiver.VData = sin(oCons) * height/2;
    
    % Draw timelines
    global timelines
    for i = 1:length(timelines)
        timelines(i).XData = [t t];
    end
end
