#ifndef SHAREDRESOURCE_H
#define SHAREDRESOURCE_H

#include <speechapi_cxx.h>
#include <memory>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Translation;



#include <speechapi_cxx.h>
#include <memory>
#include <iostream>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Translation;

class SharedResource {
public:
    static SharedResource& GetInstance() {
        static SharedResource instance;
        return instance;
    }

    std::shared_ptr<TranslationRecognizer> GetRecognizer() const {
        return recognizer;
    }

    std::shared_ptr<PushAudioInputStream> GetAudioStream() const {
        return audioStream;
    }

private:
    std::shared_ptr<TranslationRecognizer> recognizer;
    std::shared_ptr<PushAudioInputStream> audioStream;

    SharedResource() {
        
        auto format = AudioStreamFormat::GetWaveFormatPCM(32000, 16, 1); // 32 kHz, 16-bit, mono
        audioStream = AudioInputStream::CreatePushStream(format);
         auto audioConfig = AudioConfig::FromStreamInput(audioStream);
        //auto audioConfig = AudioConfig::FromStreamInput(audioStream);
        auto translationConfig = SpeechTranslationConfig::FromSubscription("ac828175134a4e8a9bf290b466ed8841", "westus");
        translationConfig->AddTargetLanguage("hi");
        translationConfig->SetSpeechRecognitionLanguage("en-US");
        

        recognizer = TranslationRecognizer::FromConfig(translationConfig, audioConfig);
        recognizer->Recognizing.Connect([](const TranslationRecognitionEventArgs& e) {
            cout << "Recognizing: " << e.Result->Text << " Translated: " << e.Result->Translations.at("hi") << endl;
            });
        recognizer->Recognized.Connect([](const TranslationRecognitionEventArgs& e) {
            if (e.Result->Reason == ResultReason::TranslatedSpeech) {
                cout << "Final translation: " << e.Result->Translations.at("hi") << endl;
                auto speechConfig = SpeechConfig::FromSubscription("ac828175134a4e8a9bf290b466ed8841", "westus");
                auto audioOutputConfig = AudioConfig::FromDefaultSpeakerOutput();
                auto synthesizer = SpeechSynthesizer::FromConfig(speechConfig, audioOutputConfig);

                auto result = synthesizer->SpeakTextAsync(e.Result->Translations.at("hi")).get();

                //if (result->Reason == ResultReason::SynthesizingAudioCompleted) {
                //    cout << "Speech synthesized to speaker." << endl;
                //}
                if (result->Reason == ResultReason::SynthesizingAudioCompleted) {
                    cout << "Speech synthesized to speaker." << endl;
                }
                else if (result->Reason == ResultReason::Canceled) {
                    auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
                    cout << "Error synthesizing speech: " << cancellation->ErrorDetails << endl;
                }
            }
            });
    }

    SharedResource(const SharedResource&) = delete;
    SharedResource& operator=(const SharedResource&) = delete;
};

#endif SHAREDRESOURCE_H