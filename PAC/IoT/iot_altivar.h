#pragma once

#include "iot_base.h"
#include "smart_ptr.h"
#include <map>
#include <string>
class modbus_client;

class altivar_node: public i_iot_node
	{
	public:
		altivar_node(unsigned int id, const char* ip, unsigned int port, unsigned long exchangetimeout);
		~altivar_node();
		void Evaluate();
		bool enabled;
		unsigned long queryinterval;

		float fc_setpoint;
		float fc_value;
		int state;
		int remote_state;
		int cmd;
		float rpm_setpoint;
		float rpm_value;
		int reverse;
		char    ip_address[16];

		enum CFG_STEP
			{
			CFG_STEP_INIT_OUTPUTS = 0,
			CFG_STEP_SET_OUTPUTS,
			CFG_STEP_INIT_INPUTS,
			CFG_STEP_SET_INPUTS,
			CFG_STEP_INIT_IOSCANNER,
			CFG_STEP_SET_IOSCANNER,
			CFG_STEP_INIT_IOPROFILE,
			CFG_STEP_SET_IOPROFILE,
			CFG_STEP_INIT_REF1,
			CFG_STEP_SET_REF1,
			CFG_STEP_INIT_CMD1,
			CFG_STEP_SET_CMD1,
			CFG_STEP_INIT_FAULTRESET,
			CFG_STEP_SET_FAULTRESET,
			CFG_STEP_INIT_SAVESETTINGS,
			CFG_STEP_SET_SAVESETTINGS,
			CFG_STEP_END = 99,
			};

		enum RUN_STEP
			{
			RUN_STEP_CHECK_CONFIG = 0,
			RUN_STEP_CONFIG = 1,
			RUN_STEP_INIT_IOSCANNER,
			RUN_STEP_QUERY_IOSCANNER,
			RUN_STEP_INIT_END = 98,
			RUN_STEP_END = 99,
			};

	protected:
		modbus_client* mc;
		bool configure;
		int querystep;
		int configurestep;
		unsigned long querytimer;
		unsigned long modbustimeout;
		int ismodbuserror;
	};

typedef std::map<std::string, altivar_node*> altivar_node_map;
typedef std::pair<std::string, altivar_node*> altivar_node_pair;

typedef std::map<unsigned int, altivar_node*> altivar_node_num_map;
typedef std::pair<unsigned int, altivar_node*> altivar_node_num_pair;

class altivar_manager
	{
	public:

		virtual ~altivar_manager();

		/// @brief ��������� ������������� ���������� ������.
		static altivar_manager* get_instance();
		void add_node(const char* IP_address, unsigned int port, unsigned int timeout);
		altivar_node* get_node(const char* IP_address);
		altivar_node* get_node(unsigned int id);
		void evaluate();
	protected:
		altivar_manager();
		altivar_node_map nodes;
		altivar_node_num_map num_nodes;
		//altivar_node** nodes;
		//u_int nodes_count;

		static auto_smart_ptr< altivar_manager > instance;
		static unsigned int index;
	};



altivar_manager* G_ALTIVAR_MANAGER();

