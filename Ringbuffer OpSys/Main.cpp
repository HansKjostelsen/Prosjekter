#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "Ringbuffer.h"

void read_thread(std::stop_token stop_token, RingBuffer<char>& buffer, int delay)
{
	while (!stop_token.stop_requested())
	{
		std::cout << buffer.pop();
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}
}

void keyboard_thread(std::stop_token stop_token, RingBuffer<char>& buffer, int delay)
{
	std::string input;
	while (!stop_token.stop_requested())
	{
		std::getline(std::cin, input);
		if (input == "exit")
		{
			return;
		}

		for (char c : input)
		{
			buffer.push(c);
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		}
	}
}

void writer_thread(std::stop_token stop_token, RingBuffer<char>& buffer, const std::string& input, int delay)
{
	while (!stop_token.stop_requested())
	{


		for (char c : input)
		{
			buffer.push(c);
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		}
	}


}



int main()
{

	//Lager en ring buffer med kapasitet 10 characters
	RingBuffer<char> buffer(10);

	//Start tr�der
	std::jthread reader(read_thread, std::ref(buffer), 100);
	std::jthread writer(writer_thread, std::ref(buffer), "0123456789", 500);
	std::jthread keyboard(keyboard_thread, std::ref(buffer), 500);

	//Main tr�der venter for brukeren til � avslutte ved � skrive "exit" i keyboard tr�den
	keyboard.join();

	writer.request_stop();
	writer.join();
	//Etter at keyboard tr�den fullf�res, stopper alle andre tr�der
	reader.request_stop();
	reader.join();

	return 0;
}