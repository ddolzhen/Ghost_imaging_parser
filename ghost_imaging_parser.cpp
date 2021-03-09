#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <fstream>
#include <string>

#define FILENAME "B4_wire_1_W0028_F03-210211-095109-1.csv"
#define PEAK_SIZE 80
#define TIME_BEFORE_PEAK 200
#define TIME_AFTER_PEAK 3e3 // This includes afterpulsing (happens 1.5-2.5 us after main peak)
#define BUFFER_SIZE 10000
#define COOLDOWN_TIME 9.93e5


struct data_entry{

	int col;
	int row;
	int tot;
	long long toa;
};

struct data_entry parseLine(char* line)
{


	char* token;
	token = strtok(line,",");
	char* ptr;
	int entry_ct=0;
	struct data_entry entry;
	while (token !=NULL)
	{
		if (entry_ct==0){
			entry.col=atoi(token);
		}else if (entry_ct==1){
			entry.row=atoi(token);
		}else if (entry_ct==2){
			entry.toa=std::stoll(token,NULL,10);
		}else if (entry_ct==3){
			entry.tot=atoi(token);
		}



		token=strtok(NULL,",");
		entry_ct++;
	}

	return entry;

}










int main(int argc, char *argv[])
{

	char* filename="";
	bool filename_found=false;



	if (argc<2)
	{
		std::cout << "No command line arguments, using default parameters\n";

	}else{
		if (argv[1][0]!='-')
		{
			filename=argv[1];
			filename_found=true;
		}else if (argv[1][0]== '-')
		{
			char command=argv[1][1];
			if (command=='f' || command == 'F')
			{
				if (argc>=3)
				{
					filename=argv[2];
					filename_found=true;
				}else{

					std::cout<<"-f must be used with a file name after\n";
					return 0xBADBEEF;
				}
			}
		}
	}
	FILE* stream;
	std::string out_file ("frames_");
	if (!filename_found)
	{
		stream = fopen(FILENAME,"r");

		for (int i=0;i<15;i++)
		{
			out_file+=FILENAME[i];
		}

	}else{
		stream = fopen(filename,"r");
		for (int i=0;i<15;i++)
		{
			out_file+=filename[i];
		}

	}

	out_file+=".txt";



	std::cout << "Writing to "<<out_file <<"\n"; 
	std::ofstream stream_out(out_file);

	std::queue<struct data_entry> data_buffer;
	char line[60];

	unsigned long frame[256][256] = {0};


	long frame_ct=0;
	struct data_entry entry;
	int* col=new int;
	int* row=new int;
	long long* toa=new long long;
	int* tot=new int;

	long long toa_prev;
	int toa_count;
	fgets(line,60,stream); // This is done to skip the header line

	long line_ct=0;
	long long toa_curr=0;
	long long toa_trig=0;
	int hits_per_toa=0;
	char saving=0;

	while (fgets(line,60,stream) ) //MAIN LOOP
	{
		char* line_tmp=strdup(line);//Use this line
		entry=parseLine(line_tmp);//Make an entry
		delete line_tmp;//Clear memory

		data_buffer.push(entry);//Push onto buffer

		if (entry.toa==toa_curr) //Counter for number of hits per time resolution period (1.56ns)
		{
			hits_per_toa++;
		}else if (entry.toa > toa_curr)
		{
			hits_per_toa=1;
			toa_curr=entry.toa;
		}else
		{
			std::cout << "WTF?!" << '\n';
			return 0xBADBEEF;
		}



		if (data_buffer.size()>BUFFER_SIZE){ data_buffer.pop(); }


		if (hits_per_toa>PEAK_SIZE && !saving)
		{
				saving=1;
				frame_ct++;
				toa_trig=toa_curr;
				std::cout<< "FOUND PEAK AT "<< ((float)(toa_curr))/4096*25<<" ns \n";

				while(!data_buffer.empty()) // Store the buffer into the matrix
				{
					entry=data_buffer.front();
					if ( (float)(toa_trig - entry.toa)/4096.0*25.0 < TIME_BEFORE_PEAK) // Add into frame if the entry is within the time window
					{
						frame[entry.col][entry.row] += entry.tot;
					}
					data_buffer.pop();
				}


				fgets(line,60,stream);
				char* line_tmp=strdup(line);//Use this line
				entry=parseLine(line_tmp);//Make an entry
				delete line_tmp;//Clear memory


				while ( float(entry.toa-toa_trig) / 4096*25.0 <TIME_AFTER_PEAK) //Write Down After Peak Hits
				{
					frame[entry.col][entry.row] += entry.tot;
					if (!fgets(line,60,stream))
					{
						std::cout<<"File ended during writing";
						return 0;
					}

					char* line_tmp=strdup(line);//Use this line
					entry=parseLine(line_tmp);//Make an entry
					delete line_tmp;//Clear memory

				}


				while (float(entry.toa-toa_trig) /4096*25 < COOLDOWN_TIME) //CD
				{
					if (!fgets(line,60,stream))
					{
						std::cout<<"FILE ENDED in cooldown";


						return 0;
					}
					char* line_tmp=strdup(line);//Use this line
					entry=parseLine(line_tmp);//Make an entry
					delete line_tmp;//Clear memory
				}
				data_buffer.push(entry);//Push onto buffer


				stream_out << "### Frame #"<<frame_ct<< " at "<< toa_trig<<'\n';
				for (int i=0;i<256;i++)
				{
					for (int j=0;j<256;j++)
					{
						stream_out << frame[j][i]<<",";
						frame[j][i]=0;
					}
					stream_out<<"\n";
				}


				saving=0;

		}





		toa_prev=*toa;
		line_ct++;
	}

	return 0;







}
