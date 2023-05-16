#include <SD.h>
#include <SPI.h>

#define sample 200
#define VCC 5

//SD
File StrainGauge_CSV; //Create a file to store the data
int num_files=0;
char name_file[20];
File root;
int countFiles(File dir);
void SD_write();

//general functions
double StrainGauge();

//general variables 
float  SG = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  //Pins
  pinMode (16, OUTPUT);
  pinMode (A7, INPUT); // Sinal

  //SD
  Serial.print("\nInitializing SD card...");
  SD.begin();
  
  root =  SD.open("/");
  int num_files = countFiles(root);
  sprintf(name_file, "/%s%d.csv", "StrainGauge_data", num_files+1);
  
  String info = "";
  info +="Tensão,";

  StrainGauge_CSV = SD.open(name_file, FILE_WRITE); //open file
  if (StrainGauge_CSV) {
    Serial.println("Done!");
    StrainGauge_CSV.println(info);
    StrainGauge_CSV.close();
}
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(16, HIGH); //Led
  
  SG = StrainGauge();
  
  // manda para o sd
  SD_write();    
}

double StrainGauge()
{
    float U_acc = 0.0;
    uint8_t U, Ux, ler_Gauge;
    
    for (int i = 0; i < sample; i++) 
    {
        ler_Gauge = analogRead(A7);
        Ux = (ler_Gauge * VCC) / 1024.0; // V = Voltagem do Map
        U_acc += Ux;               // Px_acc = acumulador do filtro 1
    }
    
    U = U_acc / (sample); // faz a média
    U_acc = 0.0; // reseta o acc 
     
    delay(200);

    return U;
}    
void SD_write()
{
  String Data = "";
   Data += float(SG);

    StrainGauge_CSV = SD.open(name_file, FILE_WRITE);
  if (StrainGauge_CSV) {
    StrainGauge_CSV.println(Data);
    StrainGauge_CSV.close();
  }

  else {
    Serial.println(F("ERRO"));
  }
}

int countFiles(File dir) {
  
  int fileCountOnSD=0; //for counting files
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      //no more files
      break;
    }
    //for each file count it 
    fileCountOnSD++;
    entry.close();
  }
  return fileCountOnSD -1;
}
