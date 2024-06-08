#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
using namespace std;

string instruction_cache[128];
string data_cache[256];

int RegFile[16];

int regInUse_new, regInUse_old, halt;

fstream file;


int PosOverflowHanlde(int num)
{
  num += 128;
  num %= 256;
  num -= 128;
  return num;
}
int SignedToUnsigned(int num)
{
    // find what the number will be if interpreted as unsigned.
    if(num < 0)
    {
        return 256+num;
    }
  return num;
}
string NumToBin(int num)
{
  // treat num as a 8 bit binary signed number and find its representation
    string bin = "00000000";
    if(num < 0)
    {
      num += 256;
    }
    for(int i = 0; i < 8; i++)
      {
        if(num % 2 == 1)
        {
          bin[7-i] = '1';
        }
        else
        {
          bin[7-i] = '0';
        }
        num /= 2;
      }
  return bin;
}
int SignedInteger(string s)
{
    int n = s.length();
    int ans = 0;
    if(s[0] == '0'){
        for(int i = 1; i < n; i++)
            {
            ans *= 2;
            ans += (int)s[i] - '0';
            }
    } else {
        for(int i = 1; i < n; i++)
            {
            ans *= 2;
            ans += 1 - ((int)s[i] - '0');
            }
        ans += 1;
        ans *= -1;
    }
    return ans;
}
int find_immediate(string instruction)
{
    return SignedInteger(instruction);
}
int find_register(string reg)
{
  int regi = 0;
  for(int i=3; i>=0; i--)
    {
      if(reg[i] == '1')
      {
        regi += 1 << (3-i);
      }
    }
  return regi;
}
string BintoHex(string bin)
{
  string upper = bin.substr(0,4);
  string lower = bin.substr(4,4);
  int upper_num = 0;
  int lower_num = 0;
  for(int i=3; i>=0; i--)
    {
      if(upper[i] == '1')
      {
        upper_num += 1 << (3-i);
      }
      if(lower[i] == '1')
      {
        lower_num += 1 << (3-i);
      }
    }
  if(upper_num > 9)
  {
    upper_num = upper_num + 'a' - 10;
  }
  else
  {
    upper_num = upper_num + '0';
  }
  if(lower_num > 9)
  {
    lower_num = lower_num + 'a' - 10;
  }
  else
  {
    lower_num = lower_num + '0';
  }

  string ans = "";
  char a = (char)upper_num;
  ans.push_back(a);
  a = (char)lower_num;
  ans.push_back(a);

  return ans;
  
}
string hexatobinary(string hexa)
{
    string output, final = "";

    for(char lower : hexa){
        if(lower == '0')
        {
            output = "0000";
        }
        else if(lower == '1')
        {
            output = "0001";
        }
        else if(lower == '2')
        {
            output = "0010";
        }
        else if(lower == '3')
        {
            output = "0011";
        }
        else if(lower == '4')
        {
            output = "0100";
        }
        else if(lower == '5')
        {
            output = "0101";
        }
        else if(lower == '6')
        {
            output = "0110";
        }
        else if(lower == '7')
        {
            output = "0111";
        }
        else if(lower == '8')
        {
            output = "1000";
        }
        else if(lower == '9')
        {
            output = "1001";
        }
        else if(lower == 'a')
        {
            output = "1010";
        }
        else if(lower == 'b')
        {
            output = "1011";
        }
        else if(lower == 'c')
        {
            output = "1100";
        }
        else if(lower == 'd')
        {
            output = "1101";
        }
        else if(lower == 'e')
        {
            output = "1110";
        }
        else if(lower == 'f')
        {
            output = "1111";
        }

        final = final + output;
    }

    return final;
}


void getDCache()
{
    file.open("input/DCache.txt", ios::in);
    for(int i = 0; i < 256; i++)
        file >> data_cache[i];
    file.close();
}
void getICache()
{
    file.open("input/ICache.txt");
    for(int i = 0; i < 128; i++){
        string x;
        file >> x;
        instruction_cache[i] = x;
        file >> x;
        instruction_cache[i] += x;
    }
    file.close();
}
void getRegFile()
{
    file.open("input/RF.txt");
    string hex;
    for(int i = 0; i < 16; i++){
        file >> hex;
        RegFile[i] = SignedInteger(hexatobinary(hex));
    }
    file.close();
}
void write_back()
{
  file.open("input/RF.txt", ios::out);
  for(int i = 0; i < 16; i++){
      file << BintoHex(NumToBin(RegFile[i]))<<endl;
  }
  file.close();
  file.open("input/DCache.txt", ios::out);
  for(int i = 0; i < 256; i++){
    file << data_cache[i]<<endl;;
  }
}


class Instruction
{
  public:
  int rs1;
  int rs2;
  int rd;
  int imm;
  string op;
  string opcode;
  string instr;
  int ALUout;
  int LMD;
  int isHalt;

   Instruction()
  {
    rs1 = -1;
    rs2 = -1;
    rd = -1;
    imm = -1;
    op = "";
    instr = "";
    ALUout = -1;
    isHalt = -1;
  }

  void fetch( int &pc )
  {
    string instr = instruction_cache[pc];
    op = hexatobinary(instr);
    pc = pc+1;
  }

  void decode( int &pc  , int &stall, int &stall_control)
  {
    stall = 0;
    string iden = op.substr(0,4);
    if(iden == "0000")
    {
      instr = "ADD";
      rd = find_register(op.substr(4,4));
      rs1 = find_register(op.substr(8,4));
      rs2 = find_register(op.substr(12,4));

      cout<<regInUse_old<<" "<<regInUse_new<<endl;

      if(rs1 == regInUse_new || rs2 == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old || rs2 == regInUse_old)
        stall = 1;
    }
    if(iden == "0001")
    {
      instr = "SUB";
      rd = find_register(op.substr(4,4));
      rs1 = find_register(op.substr(8,4));
      rs2 = find_register(op.substr(12,4));

      if(rs1 == regInUse_new || rs2 == regInUse_new){
        stall = 2;
      }
      else if(rs1 == regInUse_old || rs2 == regInUse_old)
        stall = 1;
    }
    if(iden == "0010")
    {
      instr = "MUL";
      rd = find_register(op.substr(4,4));
      rs1 = find_register(op.substr(8,4));
      rs2 = find_register(op.substr(12,4));

      if(rs1 == regInUse_new || rs2 == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old || rs2 == regInUse_old)
        stall = 1;
    }
    if(iden == "0011")
    {
      instr = "INC";
      rd = find_register(op.substr(4,4));
      rs1 = rd;
      if(rs1 == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old)
        stall = 1;
    }
    if(iden == "0100")
    {
      instr = "AND";
      rd = find_register(op.substr(4,4));
      rs1 = find_register(op.substr(8,4));
      rs2 = find_register(op.substr(12,4));

      if(rs1 == regInUse_new || rs2 == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old || rs2 == regInUse_old)
        stall = 1;
    }
    if(iden == "0101")
    {
      instr = "OR";
      rd = find_register(op.substr(4,4));
      rs1 = find_register(op.substr(8,4));
      rs2 = find_register(op.substr(12,4));

      if(rs1 == regInUse_new || rs2 == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old || rs2 == regInUse_old)
        stall = 1;
    }
    if(iden == "0110")
    {
      instr = "XOR";
      rd = find_register(op.substr(4,4));
      rs1 = find_register(op.substr(8,4));
      rs2 = find_register(op.substr(12,4));

      if(rs1 == regInUse_new || rs2 == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old || rs2 == regInUse_old)
        stall = 1;
    }
    if(iden == "0111")
    {
      instr = "NOT";
      rd = find_register(op.substr(4,4));
      rs1 = find_register(op.substr(8,4));

      if(rs1 == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old)
        stall = 1;
    }
    if(iden == "1000")
    {
      instr = "SLLI";
      rd = find_register(op.substr(4,4));
      rs1 = find_register(op.substr(8,4));
      imm = find_immediate(op.substr(12,4));

      if(rs1 == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old)
        stall = 1;
    }
    if(iden == "1001")
    {
      instr = "SRLI";
      rd = find_register(op.substr(4,4));
      rs1 = find_register(op.substr(8,4));
      imm = find_immediate(op.substr(12,4));

      if(rs1 == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old)
        stall = 1;
    }
    if(iden == "1010")
    {
      instr = "LI";
      rd = find_register(op.substr(4,4));
      imm = find_immediate(op.substr(8,8));
    }
    if(iden == "1011")
    {
      instr = "LD";
      rd = find_register(op.substr(4,4));
      rs1 = find_register(op.substr(8,4));
      imm = find_immediate(op.substr(12,4));

      if(rs1 == regInUse_new || rd == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old || rd == regInUse_old)
        stall = 1;
    }
    if(iden == "1100")
    {
      instr = "ST";
      rd = find_register(op.substr(4,4));
      rs1 = find_register(op.substr(8,4));
      imm = find_immediate(op.substr(12,4));

      if(rs1 == regInUse_new || rd == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old || rd == regInUse_old)
        stall = 1;
    }
    if(iden == "1101")
    {
      instr = "JMP";
      imm = find_immediate(op.substr(4,8));
      stall_control = 2;
    }
    if(iden == "1110")
    {
      instr = "BEQZ";
      rs1 = find_register(op.substr(4,4));
      imm = find_immediate(op.substr(8,8));
      if(rs1 == regInUse_new)
        stall = 2;
      else if(rs1 == regInUse_old)
        stall = 1;
      else 
        stall_control = 2;
    }
    if(iden == "1111")
    {
      instr = "HLT";
      halt = 1;
      isHalt = 1;
    }
    if(stall == 0){
      regInUse_old = regInUse_new;
      regInUse_new = rd;
    }

    cout<<regInUse_old<<" "<<regInUse_new<<" set\n";

    if(stall != 0 && instr == "ADD"){
      cout<<"Stalled in "<<stall<<" "<<op<<endl;
    }
  }

  void execute(int &pc)
  {
    if(instr == "ADD")
    {
      ALUout = RegFile[rs1] + RegFile[rs2];
      ALUout = PosOverflowHanlde(ALUout);
    }
    if(instr == "SUB")
    {
      ALUout = RegFile[rs1] - RegFile[rs2];
      ALUout = PosOverflowHanlde(ALUout);
    }
    if(instr == "MUL")
    {
      ALUout = RegFile[rs1] * RegFile[rs2];
      ALUout = PosOverflowHanlde(ALUout);
    }
    if(instr == "INC")
    {
      ALUout = RegFile[rs1] + 1;
      ALUout = PosOverflowHanlde(ALUout);
    }
    if(instr == "AND")
    {
      ALUout = RegFile[rs1] & RegFile[rs2];
    }
    if(instr == "OR")
    {
      ALUout = RegFile[rs1] | RegFile[rs2];
    }
    if(instr == "XOR")
    {
      ALUout = RegFile[rs1] ^ RegFile[rs2];
    }
    if(instr == "NOT")
    {
      ALUout = 255-RegFile[rs1];
    }
    if(instr == "SLLI")
    {
      ALUout = SignedToUnsigned(RegFile[rs1])<< imm;
      ALUout %= 256;
    }
    if(instr == "SRLI")
    {
      ALUout = SignedToUnsigned(RegFile[rs1])>> imm;
      ALUout %= 256;
    }
    if(instr == "LI")
    {
      ALUout = imm;
    }
    if(instr == "LD")
    {
      ALUout = RegFile[rs1]+imm;
      ALUout = PosOverflowHanlde(ALUout);
    }
    if(instr == "ST")
    {
      ALUout = RegFile[rs1]+imm;
      ALUout = PosOverflowHanlde(ALUout);
    }
    if(instr == "JMP")
    {
      pc = pc + imm - 1;
    }
    if(instr == "BEQZ")
    {
      if(RegFile[rs1] == 0)
      {
        pc = pc + imm - 1;
      }
    }
  }

  void memory(int &pc)
  {
    if(instr == "BEQZ")// cout<<"Hi\n";
    if(instr == "LD")
    {
    LMD = SignedInteger(hexatobinary(data_cache[ALUout]));
    }
    if(instr == "ST")
    {
    // register madhe -ve asel tar tyala 2's complement bin madhe convert karun tyala hexa madhe convert karun tak
      data_cache[ALUout] = BintoHex(NumToBin(RegFile[rd]));

    }
  }

  int WriteBack(int &pc)
  {
    if(instr == "ADD" || instr == "SUB" || instr == "MUL" || instr == "INC" || instr=="AND" || instr == "OR" || instr == "XOR" || instr == "NOT" || instr == "SLLI" || instr == "SRLI"  || instr == "LI")
    {
      RegFile[rd] = ALUout;
    }
    if(instr == "LD")
    {
      RegFile[rd] = LMD;
    }
    
    cout<<"prev "<<regInUse_old<<" "<<regInUse_new<<" "<<instr<<" "<<op<<"\n";

    regInUse_old = regInUse_new;
    regInUse_new = -1;

    cout<<"used "<<regInUse_old<<" "<<regInUse_new<<" "<<instr<<" "<<op<<"\n";

    if(isHalt == 1)
      return 1;
    return 0;
  }
};

int main()
{
    regInUse_new = -1;
    regInUse_old = -1;

    halt = 0;

    getDCache();

    getICache();

    getRegFile();

    int stall = 0, stall_control = 0;
    int pc = 0;

    Instruction Fetch_stage;
    Instruction Decode_stage;
    Instruction Execute_stage;
    Instruction Memory_stage;
    Instruction WriteBack_stage;

    for(int i = 0; i < 16; i++){
          // cout<<RegFile[i]<<" ";
        }
        // cout<<endl;

    int it = 20;

    while(pc < 128)
    {

        if(it == 0)
          break;
        it--;

        // ulta karayla lagel samjun gheila direct mazyashi bol kivha kritang la vichar
        WriteBack_stage = Memory_stage;
        if(WriteBack_stage.op != "")
        {
        int stop = WriteBack_stage.WriteBack(pc);
        if(stop){
          cout<<"Stopped\n";
          break;
        }
        }

        Memory_stage = Execute_stage;
        if(Memory_stage.op != "")
        {
        Memory_stage.memory(pc);
        }

        
        Execute_stage = Decode_stage;
        if(Execute_stage.op != "")
        {
        Execute_stage.execute(pc);
        }

        
        Decode_stage = Fetch_stage;
        if(Decode_stage.op != "")
        {
        Decode_stage.decode(pc, stall, stall_control);
        cout<<Decode_stage.instr<<" before\n";
        }

        if(stall_control == 0){
          if(stall == 0)
          {   
              if(halt == 0)
                Fetch_stage.fetch(pc);
              else 
                Fetch_stage.op = "";
          } else {
            Fetch_stage.op = "";
            Decode_stage.op = "";
            stall--;
          }
        } else {
          Fetch_stage.op = "";
          stall_control--;
        }
        cout<<Fetch_stage.op<<endl;

        cout<<endl;
    }
    write_back();
}
