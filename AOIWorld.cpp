#include "AOIWorld.h"

using namespace std;

AOIWorld::AOIWorld(int _x_begin, int _x_end, int _y_begin, int _y_end, int _x_count, int _y_count) :
	x_begin(_x_begin), x_end(_x_end), y_begin(_y_begin), y_end(_y_end), x_count(_x_count), y_count(_y_count)
{
	//x��������=(x���������-x����ʼ����)/x������������y��ļ��㷽ʽ��ͬ
	x_width = (x_end - x_begin) / x_count;
	y_width = (y_end - y_begin) / y_count;

	/*����������*/
	for (int i = 0; i < x_count * y_count; i++)
	{
		Grid tmp;
		m_grids.push_back(tmp);
	}
}

AOIWorld::~AOIWorld()
{
}

std::list<Player*> AOIWorld::GetSrdPlayers(Player* _player)
{
	list<Player*> ret;

	/*�����������*/
	int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY() - y_begin) / y_width * x_count;
	/*�жϾ��������ȡ���ھ�����������*/

	//���㵱ǰ������������������ĸ���
	int x_index = grid_id % x_count;
	int y_index = grid_id / x_count;

	if (x_index > 0 && y_index > 0)
	{
		list<Player*>& cur_list = m_grids[grid_id - 1 - x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (y_index > 0)
	{
		list<Player*>& cur_list = m_grids[grid_id - x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (x_index < x_count - 1 && y_index > 0)
	{
		list<Player*>& cur_list = m_grids[grid_id - x_count + 1].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (x_index > 0)
	{
		list<Player*>& cur_list = m_grids[grid_id - 1].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	list<Player*>& cur_list = m_grids[grid_id].m_players;
	ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	if (x_index < x_count - 1)
	{
		list<Player*>& cur_list = m_grids[grid_id + 1].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (x_index > 0 && y_index < y_count - 1)
	{
		list<Player*>& cur_list = m_grids[grid_id - 1 + x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (y_index < y_count - 1)
	{
		list<Player*>& cur_list = m_grids[grid_id + x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (x_index < x_count - 1 && y_index < y_count - 1)
	{
		list<Player*>& cur_list = m_grids[grid_id + 1 + x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}

	return ret;
}

bool AOIWorld::AddPlayer(Player* _player)
{
	/*�������������*/

	//������=(x-x����ʼ����)/x�������� + (y-y����ʼ����)/y����*x����������
	int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY() - y_begin) / y_width * x_count;

	/*��ӵ���������*/
	m_grids[grid_id].m_players.push_back(_player);
	return true;
}

void AOIWorld::DelPlayer(Player* _player)
{
	int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY() - y_begin) / y_width * x_count;
	m_grids[grid_id].m_players.remove(_player);
}
