function [done] = draw(startfile)
    %#  @param startfile     name of startfile
    file = fileread(startfile);
    file = splitlines(file);
    length = size(file);
    length = length(1);
    for i = 1:length
        file_line = file{i};
        file_line = strsplit(file_line,",");
        if file_line(1)=="#Body"
            ids(i) = file_line(2);
        end
    end
    length = size(ids);
    length = length(2);
    for i = 1:length
        name = ids(i)+"_output.csv"
        tmp = csvread(name);
        x = tmp(:,1);
        y = tmp(:,2);
        z = tmp(:,3);
        plot3(x,y,z);
        %text(max(tmp(:,1)),max(tmp(:,3)),max(tmp(:,4)),ids(i))
        hold on;
    end
    done = 0;
end
