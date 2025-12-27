#pragma once
#include <SDL3/SDL_audio.h>
#include <vector>
#include <iostream>

//Estructura per desar les dades d'un .wav
struct SoundData
{
	Uint8* wavData;
	Uint32 wavDataLength;
	SDL_AudioSpec spec;		//Format de l'àudio
};

enum StreamState
{
	READY = 1, PLAYING = 2, STOPPED = 3
};

class Stream
{
private:
	SDL_AudioStream* _stream;
	StreamState _state;

public:
	Stream(SDL_AudioSpec soundSpec, SDL_AudioDeviceID deviceId)
	{
		//1) Crear un stream d'àudio i:
			//- Utilitzar com a format source el mateix que el del .wav
			//- Deixar el format destí a NULL (canviarà al desitjat pel hardware en ser bindejat)
		_stream = SDL_CreateAudioStream(&soundSpec, NULL);

		//2) Bind (vincular) el stream al dispositiu d'àudio pel qual es desitja que soni
		SDL_BindAudioStream(deviceId, _stream);

		//3) Indicar estat actual
		_state = READY;
	}
	~Stream() {}

	void CheckPlayback(SoundData* soundData, std::atomic<bool>& haltRequest)
	{
		while (_state != STOPPED)
		{
			//Si ja s'ha reproduït un cop el stream i la reproducció ha acabat
			//O si cal parar el thread
			if ((_state != READY && SDL_GetAudioStreamQueued(_stream) == 0) || haltRequest)
			{
				StopStream();
			}
			//Si el stream encara no s'ha reproduït i està preparat per fer-ho
			if (_state == READY)
			{
				Uint32 bytesQueued = SDL_GetAudioStreamQueued(_stream);
				int bytesRemaining = ((int)soundData->wavDataLength) - bytesQueued;

				//"Triquinyuela" per variable-length arrays
				std::vector<Uint8> wavDataRemainingVec = std::vector<Uint8>(bytesRemaining, '\0');
				Uint8* wavDataRemaining = &wavDataRemainingVec[0];

				//Copiar memòria: dst, src, len
				SDL_memcpy(wavDataRemaining, (const Uint32*)&soundData->wavData[bytesQueued], bytesRemaining);
				SDL_PutAudioStreamData(_stream, wavDataRemaining, bytesRemaining);

				//Indicar que no es desitja afegir res més al stream fins que no acabi
				SDL_FlushAudioStream(_stream);

				//Evitar que a la següent iteració es repeteixi el so
				_state = PLAYING;
			}
		}
	}

	void CheckPlaybackLooping(SoundData* soundData, std::atomic<bool>& haltRequest)
	{
		while (_state != STOPPED)
		{
			//Si cal parar el thread
			if (haltRequest)
			{
				StopStream();
			}
			//Si el stream encara no s'ha reproduït i està preparat per fer-ho
			if (_state == READY)
			{
				Uint32 bytesQueued = SDL_GetAudioStreamQueued(_stream);
				int bytesRemaining = ((int)soundData->wavDataLength) - bytesQueued;

				//"Triquinyuela" per variable-length arrays
				std::vector<Uint8> wavDataRemainingVec = std::vector<Uint8>(bytesRemaining, '\0');
				Uint8* wavDataRemaining = &wavDataRemainingVec[0];

				//Copiar memòria: dst, src, len
				SDL_memcpy(wavDataRemaining, (const Uint32*)&soundData->wavData[bytesQueued], bytesRemaining);
				SDL_PutAudioStreamData(_stream, wavDataRemaining, bytesRemaining);

				//Indicar que no es desitja afegir res més al stream fins que no acabi
				SDL_FlushAudioStream(_stream);

				//Evitar que a la següent iteració es repeteixi el so
				_state = PLAYING;
			}
			//Si s'ha acabat el so i cal un nou cicle de reproducció
			if (_state != READY && _state != STOPPED && SDL_GetAudioStreamQueued(_stream) == 0)
			{
				SDL_ClearAudioStream(_stream);
				_state = READY;
			}
		}
	}

	void StopStream()
	{
		//1) Neteja el SDL_AudioStream
		SDL_ClearAudioStream(_stream);

		//2) Fes unbind del SDL_AudioStream
		SDL_UnbindAudioStream(_stream);

		//3) Destrueix el SDL_AudioStream
		SDL_DestroyAudioStream(_stream);

		//4) Actualitza estat
		_state = STOPPED;
	}

};