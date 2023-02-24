% Read the bmp/spo2 channel data
% Send an email/tweet

% Store the channel ID for the moisture sensor channel.
channelID = 2023802;

% Provide the ThingSpeak alerts API key.  All alerts API keys start with TAK.
alertApiKey = 'TAKqn9guEVI8SfOZ18S';

% Set the address for the HTTTP call
alertUrl="https://api.thingspeak.com/alerts/send";

% webwrite uses weboptions to add required headers.  Alerts needs a ThingSpeak-Alerts-API-Key header.
options = weboptions("HeaderFields", ["ThingSpeak-Alerts-API-Key", alertApiKey ]);

% Set the email subject.
alertSubject = sprintf("Health Report");
NumDays = 10;
ESP_Ts = 15; % esp time between two values




%%%%%%%%%%%%%%%%%%%%%%
%%%%%% Tweet %%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%
% Read the recent data.
Alert_Ts = 5; % thingspeak time between two runs
NumValues = Alert_Ts*60/ESP_Ts;
disp(NumValues);

Fields = [1, 2]; % bpm (ppg)
data = thingSpeakRead(channelID, NumDays=NumDays, Fields=Fields, ReadKey='DLRZA3TM5QHDZ4G6');
% NumDays=NumDays, NumPoints=NumValues, 

bpm_ppg  = data(:, 1);
spo2_ppg = data(:, 2);

% Check to make sure the data was read correctly from the channel.
if isempty(bpm_ppg) & isempty(spo2_ppg)
    alertBody = ' Not Active. ';
else
    % Get the values of last 15mins
    bpm_ppg = bpm_ppg(length(bpm_ppg)-NumValues+1:end);
    spo2_ppg = spo2_ppg(length(spo2_ppg)-NumValues+1:end);
%     display(spo2_ppg)
%     
%     display(bpm_ppg)
    
    % Get the most recent point in the array.
%     bbm_ppg_last = bpm_ppg(end);

    % Get max value of the 15min
    bpm_ppg_max = max(bpm_ppg); 
    bpm_ppg_min = min(bpm_ppg);
    
    spo2_ppg_max = max(spo2_ppg); 
    spo2_ppg_min = min(spo2_ppg);
    
    % Get current time
    time = datestr(datetime('now'));
    
    % Set the bpm message
    % abnormality either Brady or Tachy
    if (bpm_ppg_min<60)
        bpm_ppg_msg = "Bradycardia";
        bpm_ppg_value = bpm_ppg_min;
    elseif (bpm_ppg_max>100)
        bpm_ppg_msg = "Tachycardia";
        bpm_ppg_value = bpm_ppg_max;
    else
        bpm_ppg_msg = "Normal"; % sprintf(" Normal. (BPM: %d) (Time: %s)", bpm_ppg_value, time);
        bpm_ppg_value = bpm_ppg(end);
    end
%     disp(bpm_ppg_min);
    
    % Set the spo2 message
    if (spo2_ppg_min<85)
        spo2_ppg_msg = "Severe Hypoxia";
        spo2_ppg_value = spo2_ppg_min;
    elseif (spo2_ppg_min<90)
        spo2_ppg_msg = "Moderate Hypoxia";
        spo2_ppg_value = spo2_ppg_min;
    elseif (spo2_ppg_min<95)
        spo2_ppg_msg = "Mild Hypoxia";
        spo2_ppg_value = spo2_ppg_min;
    else
        spo2_ppg_msg = "Normal"; % sprintf(" Normal. (BPM: %d) (Time: %s)", bpm_ppg_value, time);
        spo2_ppg_value = spo2_ppg(end);
    end
    % set spo2 value to min
    
    
    % get formatted message with bpm and spo2
    alertBody = sprintf(" BPM: %d (%s!) | SpO2: %d (%s!) | (Time: %s)", bpm_ppg_value, bpm_ppg_msg, spo2_ppg_value, spo2_ppg_msg, time);
end


if ~strcmp(bpm_ppg_msg, "Normal") | ~strcmp(spo2_ppg_msg, "Normal")
    % Send tweet
    try
        webwrite('https://api.thingspeak.com/apps/thingtweet/1/statuses/update', 'api_key', 'TOM4CYDR6JIVVMYZ', 'status', alertBody);
        fprintf("[Tweet] Sent\n");
    catch someException
        fprintf("[Tweet] Failed to send alert: %s\n", someException.message);
    end
else
    fprintf(alertBody);
end




%%%%%%%%%%%%%%%%%%%%%%
%%%%%% Email %%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%
% Read the recent data.
Alert_Ts = 15; % thingspeak time between two runs
NumValues = Alert_Ts*60/ESP_Ts;
disp(NumValues);

Fields = [1, 2]; % bpm (ppg)
data = thingSpeakRead(channelID, NumDays=NumDays, Fields=Fields, ReadKey='DLRZA3TM5QHDZ4G6');
% NumDays=NumDays, NumPoints=NumValues, 

bpm_ppg  = data(:, 1);
spo2_ppg = data(:, 2);

% Check to make sure the data was read correctly from the channel.
if isempty(bpm_ppg) & isempty(spo2_ppg)
    alertBody = ' Not Active. ';
else
    % Get the values of last 15mins
    bpm_ppg = bpm_ppg(length(bpm_ppg)-NumValues+1:end);
    spo2_ppg = spo2_ppg(length(spo2_ppg)-NumValues+1:end);
%     display(spo2_ppg)
%     
%     display(bpm_ppg)
    
    % Get the most recent point in the array.
%     bbm_ppg_last = bpm_ppg(end);

    % Get max value of the 15min
    bpm_ppg_max = max(bpm_ppg); 
    bpm_ppg_min = min(bpm_ppg);
    
    spo2_ppg_max = max(spo2_ppg); 
    spo2_ppg_min = min(spo2_ppg);
    
    % Get current time
    time = datestr(datetime('now'));
    
    % Set the bpm message
    % abnormality either Brady or Tachy
    if (bpm_ppg_min<60)
        bpm_ppg_msg = "Bradycardia";
        bpm_ppg_value = bpm_ppg_min;
    elseif (bpm_ppg_max>100)
        bpm_ppg_msg = "Tachycardia";
        bpm_ppg_value = bpm_ppg_max;
    else
        bpm_ppg_msg = "Normal"; % sprintf(" Normal. (BPM: %d) (Time: %s)", bpm_ppg_value, time);
        bpm_ppg_value = bpm_ppg(end);
    end
%     disp(bpm_ppg_min);
    
    % Set the spo2 message
    if (spo2_ppg_min<85)
        spo2_ppg_msg = "Severe Hypoxia";
        spo2_ppg_value = spo2_ppg_min;
    elseif (spo2_ppg_min<90)
        spo2_ppg_msg = "Moderate Hypoxia";
        spo2_ppg_value = spo2_ppg_min;
    elseif (spo2_ppg_min<95)
        spo2_ppg_msg = "Mild Hypoxia";
        spo2_ppg_value = spo2_ppg_min;
    else
        spo2_ppg_msg = "Normal"; % sprintf(" Normal. (BPM: %d) (Time: %s)", bpm_ppg_value, time);
        spo2_ppg_value = spo2_ppg(end);
    end
    % set spo2 value to min
    
    
    % get formatted message with bpm and spo2
    alertBody = sprintf(" BPM: %d (%s!) | SpO2: %d (%s!) | (Time: %s)", bpm_ppg_value, bpm_ppg_msg, spo2_ppg_value, spo2_ppg_msg, time);
end


if ~strcmp(bpm_ppg_msg, "Normal") | ~strcmp(spo2_ppg_msg, "Normal")
     % Catch errors so the MATLAB code does not disable a TimeControl if it fails
    try
        webwrite(alertUrl , "body", alertBody, "subject", alertSubject, options);
        fprintf("[Email] Sent\n");
    catch someException
        fprintf("[Email] Failed to send alert: %s\n", someException.message);
    end
else
    fprintf(alertBody);
end