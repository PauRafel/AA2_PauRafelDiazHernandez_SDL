#pragma once
#include "Stream.h"
#include "AtomicWrapper.h"

#include "SDL3/SDL_audio.h"

#include <string>
#include <map>
#include <vector>
#include <queue>
#include <iostream>
#include <thread>
#include <exception>

//Sistema basat en els exemples oficials de SDL3
//https://github.com/libsdl-org/SDL/blob/main/examples/audio/04-multiple-streams/multiple-streams.c
//https://github.com/libsdl-org/SDL/blob/main/examples/audio/05-planar-data/planar-data.c

//Part de threads basada en:
//https://stackoverflow.com/questions/9094422/how-to-check-if-a-stdthread-is-still-running

//Per controlar quan cal terminar de cop els threads
static std::atomic<bool> shouldHaltAudio = false;
static std::vector<AtomicWrapper<bool>> threadsDone;

#define AM AudioManager::GetInstance()

class AudioManager
{
public:
	static AudioManager* GetInstance()
	{
		static AudioManager instance;
		return &instance;
	}

	//Obre dispositiu d'àudio
	bool Init()
	{
		try
		{
			_audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

			if (_audioDevice == 0)
				throw SDL_GetError();

			return true;
		}
		catch (std::exception exception)
		{
			std::cout << "Error: " << exception.what();
			return false;
		}
	}

	//Fonamental cridar abans d'acabar el programa per terminar els threads correctament
	void HaltAudio()
	{
		shouldHaltAudio = true;

		int size = threadsDone.size();
		for (int i = 0; i < size; )
		{
			//Només incrementa comptador si el thread que s'està comprovant ha acabat
			if (threadsDone.at(i)._a)
				i++;
		}

		//Reset (molt important!)
		shouldHaltAudio = false;
		threadsDone.clear();
	}

	//Llegeix i guarda dades d'un so (fitxer .wav) a partir de la seva ruta
	bool LoadSoundData(std::string path)
	{
		if (_soundsData.find(path) != _soundsData.end())
			return false;

		SoundData* soundData = new SoundData();

		try
		{
			//Carregar el fitxer .wav des de la ruta
			if (!SDL_LoadWAV(
				path.c_str(),
				&soundData->spec,			//Llegir el format
				&soundData->wavData,		//Llegir les dades d'àudio
				&soundData->wavDataLength	//Llegir la mida (en bytes) de les dades d'àudio
			))
			{
				throw SDL_GetError();
			}

			//Guardar la informació d'aquest so
			_soundsData[path] = soundData;

			return true;

		}
		catch (std::exception exception)
		{
			delete soundData;
			std::cout << "Error: " << exception.what();
			return false;
		}
	}

	//Crea un nou stream a partir de les dades del fitxer .wav associat a una ruta
	void PlaySound(std::string path)
	{
		if (_soundsData.find(path) == _soundsData.end())
			return;

		//Crear thread amb la funció corresponent
		threadsDone.push_back(std::atomic<bool>(false));
		std::thread thread(&AudioManager::PlaySoundCallback, this, path, (threadsDone.size() - 1), false);
		thread.detach();
	}

	void PlaySoundLooping(std::string path)
	{
		if (_soundsData.find(path) == _soundsData.end())
			return;

		//Crear thread amb la funció corresponent
		threadsDone.push_back(std::atomic<bool>(false));
		std::thread thread(&AudioManager::PlaySoundCallback, this, path, (threadsDone.size() - 1), true);
		thread.detach();
	}

	//Silencia / dessilencia TOT l'àudio (= pausa / ressumeix el dispositiu d'àudio)
	void Mute()
	{
		if (_muted)
			return;

		SDL_PauseAudioDevice(_audioDevice);
		_muted = true;
	}

	void Unmute()
	{
		if (!_muted)
			return;

		SDL_ResumeAudioDevice(_audioDevice);
		_muted = false;
	}

private:
	AudioManager() = default;
	AudioManager(AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = default;
	~AudioManager()
	{
		//Netejar les dades de sons
		for (std::map<std::string, SoundData*>::iterator it = _soundsData.begin(); it != _soundsData.end(); it++)
		{
			delete it->second;
		}
		_soundsData.clear();
	}

	void PlaySoundCallback(std::string path, int pos, bool looping)
	{
		Stream stream = Stream(_soundsData[path]->spec, _audioDevice);

		if (looping)
			stream.CheckPlaybackLooping(_soundsData[path], shouldHaltAudio);
		else
			stream.CheckPlayback(_soundsData[path], shouldHaltAudio);

		threadsDone[pos] = AtomicWrapper<bool>(std::atomic<bool>(true));
		std::cout << "Stopped thread" << std::endl;
	}

	bool _muted = false;
	SDL_AudioDeviceID _audioDevice = 0;

	//key: ruta al .wav
	std::map<std::string, SoundData*> _soundsData;
};