
class Packet
{
public:
	Packet();
	~Packet();
	
	template<class T>
	void Insert(T& value);

	template<class T>
	void Read(T& buff)

	void ResetReadPosition(){ _readpos = 0 };

private:
	std::vector<uint8> _data;
	uint32 _readpos;
};