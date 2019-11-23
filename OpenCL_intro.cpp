#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <iostream>
#include<CL/cl.hpp>
#include<vector>
#include<fstream>
using namespace std;

void device_info(vector<cl::Device> &devices)
{
	for (auto device : devices)
	{
		auto name = device.getInfo<CL_DEVICE_NAME>();
		auto vendor = device.getInfo<CL_DEVICE_VENDOR>();
		auto version = device.getInfo<CL_DEVICE_VERSION>();

		cout << "Name:" << name <<"   Vendor:" << vendor << "	Version:" << version << endl;
	}
}

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
	char buf[16];
	cl::Buffer devBuf(context,CL_MEM_HOST_READ_ONLY|CL_MEM_WRITE_ONLY,sizeof(buf));

	ifstream ifile("kernel_hw.txt");
	string line, ProgramSource;
	while(getline(ifile, line))
	{
		ProgramSource += line + "\n";
	}
	ifile.close();

	cl::Program::Sources sources(1, make_pair(ProgramSource.c_str(),ProgramSource.length()));
	cl::Program program(context, sources);
	auto err = program.build("-cl-std=CL1.2");
	if (err)
	{
		cout << "Error building the program. Error code: " << err;
	}
	
	cl::Kernel kernel(program, "HelloWorld");
	kernel.setArg(0, devBuf);

	cl::CommandQueue cq(context, device);
	cq.enqueueTask(kernel);
	cq.enqueueReadBuffer(devBuf, CL_TRUE, 0, sizeof(buf), buf);

	cout << buf << "\n------------------------------" << endl;

	cq.finish();
	return 1;
}
