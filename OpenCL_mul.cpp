#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <iostream>
#include<CL/cl.hpp>
#include<vector>
#include<fstream>
#include<chrono>
using namespace std;

void sequential_logic(int *A, int *B, int len);
void device_info(vector<cl::Device>& devices);

int main()
{
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	auto platform = platforms.front();

	vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	device_info(devices);

	cl::Device device = devices.front();
	cl::Context context(device);
	
	int len = 1024;
	int A[1024], B[1024], C[1024];
	for (int i=0; i<len; ++i)
	{
		A[i] = 2;
		B[i] = 3;
		C[i] = 0;
	}

	cl::Buffer Abuf(context,CL_MEM_READ_WRITE, len * sizeof(int));
	cl::Buffer Bbuf(context, CL_MEM_READ_WRITE, len * sizeof(int));
	cl::Buffer Cbuf(context, CL_MEM_READ_WRITE, len * sizeof(int));

	ifstream ifile("VectorMul.txt");
	string line, ProgramSource;
	while (getline(ifile, line))
	{
		ProgramSource += line + "\n";
	}
	ifile.close();
	cout << ProgramSource;

	cl::Program::Sources sources = { {ProgramSource.c_str(), ProgramSource.length()} };
	cl::Program program(context, sources);
	auto err = program.build("-cl-std=CL1.2");
	if (err)
	{
		cout << "Error building the program. Error code: " << err << endl;
	}

	cl::Kernel kernel(program, "VectorMul");
	kernel.setArg(0, Abuf);
	kernel.setArg(1, Bbuf);
	kernel.setArg(2, Cbuf);

	cl::CommandQueue cq(context, device, CL_QUEUE_PROFILING_ENABLE);
	cq.enqueueWriteBuffer(Abuf, CL_TRUE, 0, len * sizeof(int), A);
	cq.enqueueWriteBuffer(Bbuf, CL_TRUE, 0, len * sizeof(int), B);

	cl::Event profileEvent;

	auto start = chrono::high_resolution_clock::now();

	cq.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(len), cl::NDRange(256), nullptr, &profileEvent);
	cq.enqueueReadBuffer(Cbuf, CL_TRUE, 0, len * sizeof(int), C);
	cq.finish();

	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<std::chrono::microseconds>(stop - start);
	cout << "\nPARALLEL EXECUTION TIME : " << duration.count() << " microseconds\n" << endl;

	cl_ulong time_start;
	cl_ulong time_end;

	time_start = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_START>();
	time_end = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_END>();

	double microSeconds = (double)(time_end - time_start)/1000;
	printf("KERNEL EXECUTION TIME : %f microseconds \n", microSeconds);

	//for (int i = 0; i < len; i+=32)
	//{
	//	cout << C[i] << " ";
	//}
	cout << "\n----------------------------------------------------" << endl;

	sequential_logic(A, B, len);

	return 1;
}


void device_info(vector<cl::Device>& devices)
{
	for (auto device : devices)
	{
		auto name = device.getInfo<CL_DEVICE_NAME>();
		auto vendor = device.getInfo<CL_DEVICE_VENDOR>();
		auto version = device.getInfo<CL_DEVICE_VERSION>();
		auto CU = device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();

		cout << "Name:" << name << "   \nVendor:" << vendor << "	\nVersion:" << version << endl;
		cout << "C.U.: " << CU << endl;
	}
}

void sequential_logic(int* A, int* B, int len)
{
	vector<int> C;
	auto start = chrono::high_resolution_clock::now();
	for (int i = 0; i < len; ++i)
	{
		C.push_back(*A * *B);
	}
	auto stop = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<std::chrono::microseconds>(stop - start);

	cout << "\nSEQUENTIAL EXECUTION TIME : " << duration.count() << " microseconds\n" << endl;
	cout << "----------------------------------------------------" << endl;
}
