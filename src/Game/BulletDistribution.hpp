#ifndef BulletDistribution_INCLUDED
#define BulletDistribution_INCLUDED

#include <vector>
#include <algorithm>
#include "Core/Vector.hpp"
#include "Core/Exception.hpp"

// TODO: Documentation for this class.
// It helps bot(s) determine where to shoot.
class BulletDistribution {
public:
	BulletDistribution() {
		m_dist.reserve(m_maxPosInspected);
	}

	inline bool isReading() const {
		return m_reading;
	}

	void addPosition(Vector const& pos, Vector const& emitterPosition) {
		if (isReading()) {
			throw Exception("Distribution in read mode.");
		}
		Scalar rotation = (pos - emitterPosition).getAngleOfSlope();
		m_dist.push_back({ rotation, 0 });
	}

	bool shouldReadNow() const {
		return !isReading() && (m_dist.size() >= m_maxPosInspected);
	}

	void beginReadingDistribution() {
		if (isReading()) {
			throw Exception("Distribution already in read mode.");
		}
		std::sort(m_dist.begin(), m_dist.end());
		size_t i, j;
		size_t sz = m_dist.size();
		int numOfBullets = 1;
		for (i = 0, j = sz - 1; i <= j; ++i, --j) {
			m_dist[i].second = numOfBullets + rand() % 2;
			m_dist[j].second = numOfBullets + rand() % 2;
			numOfBullets += 1;
		}
		m_reading = true;
		m_readIndex = 0;
	}

	std::pair<Scalar, int> const& operator[](size_t index) {
		if (!isReading()) {
			throw Exception("Distribution must be in read mode first.");
		}
		if (m_readIndex >= m_dist.size()) {
			throw Exception("Index out of range. No more distribution to read.");
		}
		return m_dist[index];
	}

	bool readNext(Scalar* rotationPtr, int* numOfBulletsPtr) {
		if (!isReading()) {
			throw Exception("Distribution must be in read mode first.");
		}
		if (m_readIndex >= m_dist.size()) {
			return false;
		}
		std::pair<Scalar, int> const& pair = m_dist[m_readIndex++];
		*rotationPtr = pair.first;
		*numOfBulletsPtr = pair.second;
		return true;
	}

	void endReadingDistribution() {
		if (!isReading()) {
			throw Exception("Distribution already in write mode.");
		}
		m_dist.clear();
		m_reading = false;
	}

private:
	size_t const m_maxPosInspected = 100;
	std::vector<std::pair<Scalar, int>> m_dist{};
	bool m_reading{ false };
	size_t m_readIndex{};
};

#endif // BulletDistribution_INCLUDED
