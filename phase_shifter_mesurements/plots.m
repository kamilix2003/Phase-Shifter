
% Get a list of all s2p files in the current directory
fileList = dir('*.s2p');
numFiles = length(fileList);

figure;

for k = 1:numFiles
    % Load the s-parameters from the current file
    sobj = sparameters(fileList(k).name);
        
    % Plot S11
    rfplot(sobj, 1, 1); hold on;
    title(['S11 - ' fileList(k).name]);
    
end

figure;

for k = 1:numFiles
    % Load the s-parameters from the current file
    sobj = sparameters(fileList(k).name);
    
    % Plot S22
    rfplot(sobj, 2, 2); hold on;
    title(['S22 - ' fileList(k).name]);
    
end
