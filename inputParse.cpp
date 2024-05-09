#include <bits/stdc++.h>

struct inputPin{
    std::string inputName;
    double x, y;
};

struct outputPin{
    std::string outputName;
    double x, y;
};

struct Pin{
    std::string pinName;
    double x, y;
};

struct Flipflop{
    std::string ffName;
    int bits, pinCount;
    double width, height;
    Pin* pins;
    double qDelay; //todo
    double power; //todo
};

struct Gate{
    std::string gateName;
    int pinCount;
    double width, height;
    Pin* pins;
};

struct Instance{
    std::string instName, cellName;
    double x, y;
    double timingSlack; //todo
};

struct NetPin{
    std::string netPinName;
};

struct Net{
    std::string netName;
    int pinCount;
    NetPin* pins;
};

struct PlacementRow{
    double startX, startY, width, height, sitesNum;
};

struct qPinDelay{
    double delay;
    std::string ff_name;
};

struct TimingSlack{
    std::string cellName, pinName;
    double slack;
};

struct GatePower{
    double power;
    std::string cellName;
};

int main(int argc, char const *argv[]){
    //i/o
    std::ifstream inputFile(argv[1]);
    if(!inputFile.is_open()){
        std::cout << "no input file\n";
        return -1;
    }

    std::ofstream outputFile(argv[2]);
    if(!outputFile.is_open()){
        std::cout << "no output file\n";
        return -1;
    }

    //cost function parameters
    double a, b, g, l;
    // dieSize
    double low_left_x, low_left_y, up_right_x, up_right_y;
    // input pin count, output pin count
    int inputCount, outputCount;
    //input pins, output pins;
    inputPin inputPins[1000]; 
    outputPin outputPins[1000];
    //cell library
    int ff_num = 0, gate_num = 0;
    Flipflop flipflops[1000];
    Gate gates[1000]; 

    //num of instances
    int numInstance = 0;
    Instance* instances;

    //num of netlist, netlist;
    int numNets = 0;
    Net* netlist;

    //bin width, util, max
    double binWidth, binHeight, binMaxUtil;

    //placement rows;
    int placementRowNum = 0;
    PlacementRow placementRows[1000];

    //displacement delay
    double displacement_delay;

    //q-pin delay
    qPinDelay* qPinDelays;
    
    //timing slack
    std::vector<TimingSlack> TimingSlacks;

    //gate Power
    std::vector<GatePower> gatePowers;


    std::string param;

    //alpha
    inputFile >> param >> a;
    outputFile << "param: " << param << " " << a << "\n";
    //beta
    inputFile >> param >> b;
    outputFile << "param: " << param << " " << b << "\n";
    //gamma
    inputFile >> param >> g;
    outputFile << "param: " << param << " " << g << "\n";
    //lambda
    inputFile >> param >> l;
    outputFile << "param: " << param << " " << l << "\n";

    //dieSize
    inputFile >> param >> low_left_x >> low_left_y >> up_right_x >> up_right_y;
    outputFile << "param: " << param << " " << low_left_x << " " << low_left_y << " " << up_right_x << " " << up_right_y << "\n";
    //inputCount
    inputFile >> param >> inputCount;
    for(int i = 0; i < inputCount; i++){
        inputPin newInput;
        inputFile >> param >> newInput.inputName >> newInput.x >> newInput.y;
        inputPins[i] = newInput;
    }

    for(int i = 0;i < inputCount; i++){
        outputFile << "input pin: " << inputPins[i].inputName << " " << inputPins[i].x << " " << inputPins[i].y << "\n";
    }
    //outputCount
    inputFile >> param >> outputCount;
    for(int i = 0; i < outputCount; i++){
        outputPin newInput;
        inputFile >> param >> newInput.outputName >> newInput.x >> newInput.y;
        outputPins[i] = newInput; 
    }

     for(int i = 0;i < outputCount; i++){
        outputFile << "output pin: " << outputPins[i].outputName << " " << outputPins[i].x << " " << outputPins[i].y << "\n";
    }

    // Cell library
    cell_input:
    inputFile >> param;
    if(param.compare("FlipFlop") == 0){
        Flipflop ff;
        inputFile >> ff.bits >> ff.ffName >> ff.width >> ff.height >> ff.pinCount;
        ff.pins = new Pin[ff.pinCount];

        for(int i = 0;i < ff.pinCount; i++){
            Pin p;
            inputFile >> param >> p.pinName >> p.x >> p.y;
            ff.pins[i] = p;
        }

        flipflops[ff_num] = ff;
        ff_num += 1; 
        goto cell_input;
    } else if(param.compare("Gate") == 0){
        Gate g;
        inputFile >> g.gateName >> g.width >> g.height >> g.pinCount;
        g.pins = new Pin[g.pinCount];

        for(int i = 0;i < g.pinCount; i++){
            Pin p;
            inputFile >> param >> p.pinName >> p.x >> p.y;
            g.pins[i] = p;
        }
        gates[gate_num] = g;
        gate_num += 1;
        goto cell_input;
    } else {
        goto num_instance;
    }

    num_instance:
    outputFile << "ff_num: " << ff_num << "\n";
    for(int i = 0;i < ff_num; i++){
        int p_count = flipflops[i].pinCount;
        outputFile << "FlipFlop " << flipflops[i].bits << " " << flipflops[i].ffName << " " << flipflops[i].width << " "  << flipflops[i].height << " " << flipflops[i].pinCount << '\n';
        for(int j = 0 ; j < p_count; j++){
            outputFile << "Pin " << flipflops[i].pins[j].pinName << " " << flipflops[i].pins[j].x << " " << flipflops[i].pins[j].y << '\n';
        }
    }

    outputFile << "gate_num: " << gate_num << "\n";
    for(int i = 0;i < gate_num; i++){
        int p_count = gates[i].pinCount;
        outputFile << "Gate " << gates[i].gateName << " " << gates[i].width << " "  << gates[i].height << " " << gates[i].pinCount << '\n';
        for(int j = 0 ; j < p_count; j++){
            outputFile << "Pin " << gates[i].pins[j].pinName << " " << gates[i].pins[j].x << " " << gates[i].pins[j].y << '\n';
        }
    }

    //num instance
    inputFile >> numInstance;
    // std::cout << numInstance << '\n';
    instances = new Instance[numInstance];

    for(int i = 0;i < numInstance; i++){
        Instance inst;
        inputFile >> param >> inst.instName >> inst.cellName >> inst.x >> inst.y;
        instances[i] = inst;
    }

    for(int i = 0;i < numInstance; i++){
        outputFile << "Inst " << instances[i].instName << " " << instances[i].cellName << " " << instances[i].x << " " << instances[i].y << "\n";
    }

    //netlist
    inputFile >> param >> numNets;
    netlist = new Net[numNets];
    for(int i = 0;i < numNets; i++){
        Net n;
        inputFile >> param >> n.netName >> n.pinCount;

        n.pins = new NetPin[n.pinCount];
        for(int j = 0; j < n.pinCount; j++){
            NetPin p;
            inputFile >> param >> p.netPinName;
            n.pins[j] = p;
        }
        netlist[i] = n;
    }


    for(int i = 0;i < numNets; i++){
        outputFile << "Net " << netlist[i].netName << " " << netlist[i].pinCount << '\n';
        
        int pinNum = netlist[i].pinCount;

        for(int j = 0; j < pinNum; j++){
            outputFile << "Pin " << netlist[i].pins[j].netPinName << "\n";
        }
    }

    //bin width, height, max_util;
    inputFile >> param >> binWidth;
    outputFile << param << " " <<  binWidth << "\n";
    inputFile >> param >> binHeight;
    outputFile << param << " " <<  binHeight << "\n";
    inputFile >> param >> binMaxUtil;
    outputFile << param << " " <<  binMaxUtil << "\n";

    //plaxement rows
    input_placement_row:
    inputFile >> param;
    if(param.compare("PlacementRows") == 0){
        PlacementRow pr;
        inputFile >> pr.startX >> pr.startY >> pr.width >> pr.height >> pr.sitesNum;

        placementRows[placementRowNum++] = pr;
        goto input_placement_row;
    } else if(param.compare("DisplacementDelay")){
        goto displacement_delay;
    }
    displacement_delay:
    //displacement delay
    inputFile >> displacement_delay;
    

    //Todo: Directly place the qPinDelay in the FF struct
    //q-pin delay
    qPinDelays = new qPinDelay[ff_num];
    for(int i = 0; i < ff_num; i++){
        qPinDelay q;
        inputFile >> param >> q.ff_name >> q.delay;

        qPinDelays[i] = q;
    }

    //Todo: Directly place the timingSlack in the Instance Struct
    //timing slack
    input_timing_slack:
    int temp_pos = inputFile.tellg();
    inputFile >> param;
    if(param.compare("TimingSlack") == 0){
        TimingSlack t;
        inputFile >> t.cellName >> t.pinName >> t.slack;

        TimingSlacks.push_back(t);
        goto input_timing_slack;
    } else{
        inputFile.seekg(temp_pos);
        goto exit;
    }

    exit:
    //gate power
    //Todo: Directly place the gatePower in the FF struct;
    for(int i = 0; i< ff_num; i++){
        GatePower g;
        inputFile >> param >> g.cellName >> g.power;

        gatePowers.push_back(g);
    }


    
    for(int i = 0;i < placementRowNum; i++){
        outputFile << "PlacementRows " << placementRows[i].startX << " " << placementRows[i].startY << " " << placementRows[i].width << " " << placementRows[i].height << " " << placementRows[i].sitesNum << '\n'; 
    }

    for(int i = 0 ;i < ff_num; i++){
        outputFile << "QpinDelay " << qPinDelays[i].ff_name << " " << qPinDelays[i].delay << '\n';
    }

    for(int i = 0; i < TimingSlacks.size(); i++){
        outputFile << "TimingSlack " << TimingSlacks[i].cellName << " " << TimingSlacks[i].pinName << " " << TimingSlacks[i].slack << '\n';
    }
    
    for(auto it: gatePowers){
        outputFile << "GatePower " << it.cellName << " " << it.power << '\n';
    }

    inputFile.close();
    outputFile.close();
}