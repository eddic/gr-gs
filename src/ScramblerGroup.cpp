#include "ScramblerGroup.hpp"

void gr::Isatec::GuidedScrambling::ScramblerGroup::handler(HandlerArguments& args, const HandlerConstArguments& cargs)
{
	std::unique_lock<std::mutex> countLock(args.countMutex, std::defer_lock);
	std::unique_lock<std::mutex> sleepLock(args.sleepMutex);
	while(cargs.sleep)
	{
		sleepLock.unlock();

		scramble(cargs.divider, *cargs.input, cargs.remainder, *cargs.feedback, cargs.constellation);

		countLock.lock();
		--args.count;
		args.countCV.notify_one();

		sleepLock.lock();
		countLock.unlock();
		args.sleepCV.wait(sleepLock);
	}
}

void gr::Isatec::GuidedScrambling::ScramblerGroup::configure(
		const unsigned int length,
		unsigned int indexStart,
		const unsigned int indexEnd,
		const unsigned int augmentingLength,
		const unsigned int remainderLength,
		const unsigned int method,
		const unsigned int fieldSize)
{
	m_scramblers.resize(indexEnd-indexStart);
	for(auto& scrambler: m_scramblers)
		scrambler.configure(length, indexStart++, augmentingLength, remainderLength, method, fieldSize);
	m_winner = &m_scramblers.front();
}
