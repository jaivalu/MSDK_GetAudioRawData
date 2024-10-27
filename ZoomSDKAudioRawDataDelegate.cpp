#include "windows.h"
#include <cstdint>
#include <iostream>
#include <fstream>
#include "rawdata/rawdata_audio_helper_interface.h"
#include "ZoomSDKAudioRawDataDelegate.h"
#include "zoom_sdk_def.h"
#include "SharedResource.h"


using namespace std;
using namespace ZOOM_SDK_NAMESPACE;

struct AudioBuffer {
	char* data;   // Pointer to the audio data
	size_t size;  // Size of the audio data in bytes
};


void ZoomSDKAudioRawDataDelegate::onOneWayAudioRawDataReceived(AudioRawData* audioRawData, uint32_t node_id)
{
	//std::cout << "Received onOneWayAudioRawDataReceived" << std::endl;
	//add your code here
}
void ZoomSDKAudioRawDataDelegate::onShareAudioRawDataReceived(AudioRawData* data_)
{
}
void ZoomSDKAudioRawDataDelegate::onMixedAudioRawDataReceived(AudioRawData* audioRawData)
{
	auto& sharedResource = SharedResource::GetInstance();
	auto audioStream = sharedResource.GetAudioStream();
	auto buffer = audioRawData->GetBuffer();

	audioStream->Write(reinterpret_cast<uint8_t*>(audioRawData->GetBuffer()), audioRawData->GetBufferLen());
	//std::cout << "Received onMixedAudioRawDataReceived" << std::endl;
	//add your code here


	static ofstream pcmFile;
	pcmFile.open("audio.pcm", ios::out | ios::binary | ios::app);

	if (!pcmFile.is_open()) {
		std::cout << "Failed to open wave file" << std::endl;
		return;
	}
	try {
		// Write the audio data to the file
		pcmFile.write((char*)audioRawData->GetBuffer(), audioRawData->GetBufferLen());
		std::cout << "buffer length: " << audioRawData->GetBufferLen() << std::endl;
		std::cout << "buffer : " << audioRawData->GetBuffer() << std::endl;

		// Close the wave file
		pcmFile.close();
		pcmFile.flush();
	}
	catch (exception e)
	{
		std::cout << "Failed to write wave file" << std::endl;
	}


}