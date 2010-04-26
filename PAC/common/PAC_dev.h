/// @file PAC_dev.h
/// @brief �������� �������� �������, ������� ��������� ������� �������� 
/// ��������� ��������� PAC �� ������. ����� ����������� ������ ��� ������ � 
/// ������������ ( �������, ������,... ). ������ ������������ ������ � 
/// ����������� ( PAC ).
/// 
/// @author  ������ ������� ���������.
///
/// @par �������� �������� �������������:
/// @c DEBUG   - ���������� c ������� ���������� ���������� � �������.
/// 
/// @par ������� ������:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PAC_DEVICES_H
#define PAC_DEVICES_H

#include <string.h>
#include <stdio.h>

#include "sys.h"
#include "wago.h"

#include "g_device.h"

//#include "param_ex.h"
//-----------------------------------------------------------------------------
/// @brief ������ ������, ������� ������������ ��� �������� ��������� ���������,
/// ������� �������������� �������� ���������� ����.
//
/// ��������: ���������, ��������� � �.�.
template < class data_type > class array_device: public i_simple_device
    {
    public:
        /// @param n        - �����.
        /// @param new_name - ���.
        /// @param new_subdev_cnt - ���������� ��������� � �������.
        /// @param type     - ���. ��� ���������� ��������� � �������� ��
        /// ������.
        array_device( u_int_4 n, 
            const char *new_name,
            u_int_2 new_subdev_cnt,
            char type ): sub_dev_cnt( new_subdev_cnt ), 
            type( type ), 
            n( n )
            {
            name = new char[ strlen( new_name ) + 1 ];
            strcpy( name, new_name );

            prev_val = new data_type[ sub_dev_cnt ];
            for ( unsigned int i = 0; i < sub_dev_cnt; i++ )
                {
                prev_val[ i ] = 0;
                }              
            }

        /// @brief ���������� ���������� � �������� �����.
        ///
        /// ��� �������� ���������� �� ������.
        ///
        /// @param buff [out] - ��������� �� �������� �����, ���� �����
        /// ����������� ���������� ����������.
        ///
        /// @return - ���������� ����������� ����.
        int  save_device( char *buff )
            {
            // ������ ������ (buff) � ��������� ����:
            //    1 ����  - ���;                                (1)
            //    4 ����� - �����;                              (2)
            //    1 ����  - ����� ����� ������ ����������;      (3)
            //    � ����  - ��� ������ ����������;              (4)
            //    4 ����� - ���������� ������������;            (5)

            u_int_2 idx = 0;

            buff[ idx++ ] = type;                               //(1)
            ( ( u_int_4* ) ( buff + idx ) )[ 0 ] = n;           //(2)
            idx += 4;
            buff[ idx++ ] = strlen( name );                     //(3)
            strcpy( buff + idx, name );                         //(4)
            idx += strlen( name ) + 1;
            ( ( u_int_4* ) ( buff + idx ) )[ 0 ] = sub_dev_cnt; //(5)
            idx += 4;           

            return idx;
            }

        /// @brief ���������� ��������� ���������� � �������� �����.
        ///
        /// ��� �������� ��������� ���������� �� ������.
        ///
        /// @param buff [out] - ��������� �� �������� �����, ���� �����
        /// ����������� ���������� ��������� ����������.
        ///
        /// @return - ���������� ����������� ����.
        int  save_state( char *buff )
            {
            // ������ ������ (buff) � ��������� ����:
            //  4 ����� - ����� ����������;                     (1)
            //  4 ����� - ���������� ������������;              (2)
            //  �����   - ������ ������� �������������.
            ( ( u_int_4* ) buff )[ 0 ] = n;                     //(1)
            ( ( u_int_4* ) buff )[ 1 ] = sub_dev_cnt;           //(2)
            u_int_2 answer_size = 8;

            for ( u_int_4 i = 0; i < sub_dev_cnt; i++ )
                {
                ( ( data_type* ) ( buff + 8 ) )[ 0 ] = 
                    get_val( i );
                buff += sizeof( data_type );
                prev_val[ i ] = get_val( i );
                answer_size += sizeof( data_type );      
                }      

            return answer_size;
            }

        /// @brief ���������� ������������� ��������� ���������� � �������� �����.
        ///
        /// ��� �������� ��������� ���������� �� ������.
        ///
        /// @param buff [out] - ��������� �� �������� �����, ���� �����
        /// ����������� ���������� ��������� ����������.
        ///
        /// @return - ���������� ����������� ����.
        int  save_changed_state( char *buff )
            {
            // ������ ������ (buff) � ��������� ����:          
            //  2 ����� - ���������� ���������, ���������� ���� ���������;  (1)
            //  2 ����� - ����� ���������� � ������� ���������.             (2)
            //  �����   - ��� ���������� ���������                          (3)
            // ��������� ���������� �������� ��� ���������� ��������.       (4)
            //�������� ������ ������.                                       (5)
            //����������� �� 1 ���������� ���������, ���������� ����
            //���������.                                                    (6)
            //���� ��� ���������, ���������� ���� ���������, ���������� 0.  (7)
            u_int_2 *changed_dev_cnt = ( u_int_2* ) ( buff );               //1
            *changed_dev_cnt = 0;
            u_int_2 answer_size = 2;

            for ( u_int_2 i = 0; i < sub_dev_cnt; i++ )
                {
                if ( get_val( i ) != prev_val[ i ] )
                    {
                    ( ( u_int_2* ) ( buff + answer_size ) )[ 0 ] = i;       //2
                    answer_size += 2;
                    ( ( data_type* ) ( buff + answer_size ) )[ 0 ] =        //3
                        get_val( i );
                    prev_val[ i ] = get_val( i );                           //4
                    answer_size += sizeof( data_type );                     //5
                    ( *changed_dev_cnt )++;                                 //6
                    }
                }
            if ( 2 == answer_size )                                         //7
                {
                return 0;
                }

            return answer_size;              
            }

        /// @brief ��������� �������� �������� ����� ������. 
        ///
        /// ��� �������� �����, ��������, ����� ������������ ��� ���������� 
        /// ������� � ����� ( @ref save_state ).
        ///
        /// @param idx - ������ ��������.
        ///
        /// @return - �������� �������� � �������� @ref idx.
        virtual data_type get_val( int idx ) = 0;

        /// @brief ����� ������� � �������.
        ///
        /// ��� ������������� � ���������� �����.
        void print() const
            {
            char tmp_str[ 100 ];    

            if ( strlen( name ) < 8 ) 
                {
                sprintf( tmp_str, "\"%s\", \t\t[ %3lu ]", 
                    name, ( unsigned long int ) sub_dev_cnt );
                }
            else sprintf( tmp_str, "\"%s\", \t[ %3lu ]", 
                name, ( unsigned long int ) sub_dev_cnt );

            print_str( tmp_str );
            }

        /// @brief ����������-�������� ���������� ������ @ref i_simple_device.
        u_int_4 get_n() const
            { 
            return 0; 
            } 

    protected:
        u_int_4         sub_dev_cnt;    ///< ���������� ������������.
        char            *name;          ///< ���.
        char            type;           ///< ���.
        u_int_4         n;              ///< ���������� �����.

        data_type*      prev_val;       ///< ������ ���������� ��������.
    };
//-----------------------------------------------------------------------------
/// @brief ����� �������������� �������� ����������, ������� ������������ � 
/// �������.
class device : public i_simple_device
    {
    public:
        enum CONSTANTS
            {
            C_DEVICE_TYPE_CNT = 14,     ///< ���������� ����� ���������.
            };

        /// ���� ���������.
        enum DEVICE_TYPE
            {
            DT_V = 0,   ///< ������. 
            DT_N = 5,   ///< �����.
            DT_M,       ///< �������.
            DT_LS,      ///< ������� (����/���).
            DT_TE,      ///< �����������.
            DT_FE,      ///< ������ (��������).
            DT_FS,      ///< ������ (����/���).
            DT_CTR,     ///< �������.
            DT_AO,      ///< ���������� �����.
            DT_LE,      ///< ������� (��������).
            DT_FB,      ///< �������� �����.
            DT_UPR,     ///< ����� ����������.
            DT_QE,      ///< ������������.
            DT_AI,      ///< ���������� ����.
            };

        /// ������� ���������.
        enum DEVICE_SUB_TYPE
            {
            DST_NONE = 0,       ///< ������ �����������.

            //DT_V = 0, 
            DST_V_1DO = 0,      ///< ������ � ����� ������� ����������.
            DST_V_2DO,          ///< ������ � ����� �������� ����������.
            DST_V_1DO_1DI,      ///< ������ � ����� ������� ���������� � ����� �������� ������.
            DST_V_1DO_2DI,      ///< ������ � ����� ������� ���������� � ����� ��������� �������.
            DST_V_2DO_2DI,      ///< ������ � ����� �������� ���������� � ����� ��������� ������� (��������).
            DST_V_1DO_3DI,      ///< ������ � ����� ������� ���������� � ����� ��������� �������.
            DST_V_1DO_2DI_S,    ///< ������ � ����� ������� ���������� � ����� ��������� ������� �� ���� �� ���������.
            DST_V_AS_MIX,       ///< ������ � ����� �������� ���������� � ����� ��������� ������� � AS ����������� (��������).
            };

        device();

        /// @brief ��������� ����������.
        ///
        /// ��������� ���������� � �������� ���������. ��� ������� ��� ��������
        /// ��� �������������, �� ���� ���� �� ��������� �������� - ��������.
        virtual void on() = 0;

        /// @brief ���������� ����������.
        ///
        /// ��������� ���������� � ��������� ���������. ��� ������� ��� ��������
        /// ��� ���������������, �� ���� ���� �� ��������� �������� - ��������.
        virtual void off() = 0;

        /// @brief ��������� �������� ��������� ����������.
        ///
        /// @return - ������� ��������� ���������� � ���� ������ �����.
        virtual int get_state() = 0;

        /// @brief ��������� ������ ��������� ����������.
        ///
        /// @param new_state - ����� ��������� �������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        virtual int set_state( int new_state ) = 0;

        /// @brief ��������� �������� ��������� ����������.
        ///
        /// @return - ������� ��������� ���������� � ���� �������� �����.
        virtual float get_value() = 0;

        /// @brief ��������� �������� ��������� ����������.
        ///
        /// @param new_value - ����� ��������� ����������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        virtual int set_value( float new_value ) = 0;

        /// @brief ���������� ������ ���������� � �����.
        ///
        /// @param buff - ����� ��� ���������� ����������.
        ///
        /// @return -  ���������� ���������� ����.
        int save_device( char *buff  );

        /// @brief ����� ������� � �������.
        ///
        /// ��� ������������� � ���������� �����.
        void print() const;

        /// @brief �������� ������ ���������� �� ������.
        ///
        /// @param cfg_file - ���������� ��������� ���������� ����� � ���������
        ///   ����������.
        ///
        /// @return - 0 - ��.
        virtual int load( file *cfg_file );

        /// @brief ��������� ������ ����������.
        ///
        /// @return -  ����� ����������.
        u_int_4 get_n() const;

    protected:
        u_int_4 number;             ///< ����� ����������.

        DEVICE_TYPE     type;       ///< ��� ����������.
        DEVICE_SUB_TYPE sub_type;   ///< ������ ����������.
    }; 
//-----------------------------------------------------------------------------
/// @brief ����������, ��� �������� ��������� �������� ��������� ���� ����.
///
/// ��� �������, ������, �������, �������� ����� � �.�.
class char_state_device : public device
    {
    public:
        /// @brief ���������� ���������� @ref i_save_device.
        int save_changed_state( char *buff );

        /// @brief ���������� ���������� @ref i_save_device.
        int save_state( char *buff  );

    private:
        char prev_state;    ///< ���������� ��������� ����������.
    };
//-----------------------------------------------------------------------------
/// @brief ����������, ��� �������� ��������� �������� ���������� 4 �����.
///
/// ��� �������.
class u_int_4_state_device : public device
    {
    public:
        /// @brief ���������� ���������� @ref i_save_device.
        int save_changed_state( char *buff );

        /// @brief ���������� ���������� @ref i_save_device.
        int save_state( char *buff  );

        /// @brief ��� ���������� ��������� ����������.                
        virtual u_int_4 get_u_int_4_state() = 0;

    private:
        u_int_4 prev_state;    ///< ���������� ��������� ����������.
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ���������� ������. 
///
/// ���������� ���� �������� �����, ������ � �.�. ��������� ������ ���������.
class DI_device
    {
    public:
        /// @brief ��������� �������� ��������� ����������.
        ///
        /// @return - ������� ��������� ���������� � ���� ������ �����.
        virtual int get_state() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ���������� �������. 
///
/// ���������� ���� �������, ������� � �.�. ��������� ������ ���������.
class DO_device: public DI_device
    {
    public:
        /// @brief ��������� ����������.
        ///
        /// ��������� ���������� � �������� ���������. ��� ������� ��� ��������
        /// ��� �������������, �� ���� ���� �� ��������� �������� - ��������.
        virtual void on() = 0;

        /// @brief ���������� ����������.
        ///
        /// ��������� ���������� � ��������� ���������. ��� ������� ��� ��������
        /// ��� ���������������, �� ���� ���� �� ��������� �������� - ��������.
        virtual void off() = 0;

        /// @brief ��������� ������ ��������� ����������.
        ///
        /// @param new_state - ����� ��������� �������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        virtual int set_state( int new_state ) = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ���������� ������. 
///
/// ���������� ���� �����������, ������� � �.�. ��������� ������ ���������.
class AI_device
    {
    public:
        /// @brief ��������� �������� ��������� ����������.
        ///
        /// @return - ������� ��������� ���������� � ���� �������� �����.
        virtual float get_value() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ���������� �������. 
///
/// ���������� ���� ����������� ������ ���������� � �.�. ��������� ������
/// ���������.
class AO_device: public AI_device
    {
    public:
        /// @brief ���������� ����������.
        ///
        /// ��������� ���������� � ��������� ���������. ��� ������� ��� ��������
        /// ��� ���������������, �� ���� ���� �� ��������� �������� - ��������.
        virtual void off() = 0;

        /// @brief ��������� �������� ��������� ����������.
        ///
        /// @param new_value - ����� ��������� ����������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        virtual int set_value( float new_value ) = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ���������� �� ������ ������� �����/������ WAGO. 
/// 
/// � ����� ������ � ���������� ����� ���� ���� ��� ��������� �������
/// �����/������ (���������� ��� ����������).
class wago_device
    {
    public:
        /// @brief �������� ������ ���������� �� ������.
        ///
        /// @param cfg_file - ���������� ��������� ���������� ����� � ���������
        ///   ����������.
        ///
        /// @return -  ���������� ��������� ����.
        virtual int load( file *cfg_file );

    protected:
        /// @brief ��������� ��������� ������ ����������� ������.
        ///
        /// @param index - ������ ������ � ������� ���������� �������� ������� 
        /// ����������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        int get_DO( u_int index );

        /// @brief ��������� ��������� ������ ����������� ������.
        ///
        /// @param index - ������ ������ � ������� ���������� �������� ������� 
        /// ����������.
        /// @param value - ����� ��������� ������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        int set_DO( u_int index, char value );

        /// @brief ��������� ��������� ������ ����������� �����.
        ///
        /// @param index - ������ ������ � ������� ���������� ������� ������� 
        /// ����������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        int get_DI( u_int index );

        /// @brief ��������� ��������� ������ ����������� �����.
        ///
        /// @param index - ������ ������ � ������� ���������� ������� ������� 
        /// ����������.
        /// @param value - ����� ��������� ������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        int set_DI( u_int index, char value );

        /// @brief ��������� ��������� ������ ����������� ������.
        ///
        /// @param index - ������ ������ � ������� ���������� �������� ������� 
        /// ����������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        float get_AO( u_int index );

        /// @brief ��������� ��������� ������ ����������� ������.
        ///
        /// @param index - ������ ������ � ������� ���������� �������� ������� 
        /// ����������.
        /// @param value - ����� ��������� ������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        int   set_AO( u_int index, float value );

        /// @brief ��������� ��������� ������ ����������� �����.
        ///
        /// @param index - ������ ������ � ������� ���������� ������� ������� 
        /// ����������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        float get_AI( u_int index );

        /// @brief ��������� ��������� ������ ����������� �����.
        ///
        /// @param index - ������ ������ � ������� ���������� ������� ������� 
        /// ����������.
        /// @param value - ����� ��������� ������.
        ///
        /// @return -  0 - ��.
        /// @return - >0 - ������.
        int   set_AI( u_int index, float value );

        virtual void print() const;

    private:
        /// @brief ������ ������� �����/������ ����������.
        struct IO_channels
            {
            u_int  count;   ///< ���������� �������.
            u_int* tables;  ///< ������ ������.
            u_int* offsets; ///< ������ �������� � �������� ������.

            IO_channels() : count( 0 ), tables( 0 ), offsets( 0 )
                {
                }
            };

        IO_channels DI_channels;    ///< ������ ����������� �����.
        IO_channels DO_channels;    ///< ������ ����������� ������.    
        IO_channels AI_channels;    ///< ������ ����������� �����.
        IO_channels AO_channels;    ///< ������ ����������� ������.

        u_int   params_count;
        float   *params;

        void print_table( const char *str, const IO_channels &channels ) const;

        /// @brief �������� ���������� � ������ ������� �����/������ �� 
        /// ��������� ������.
        /// 
        /// @param stream - ����� ��� ���������� ����������.
        /// @param [out] channels - ������, � ������� �����������.
        ///
        /// @return -  ���������� ��������� ����.
        int load_table_from_string( char *str, IO_channels &channels );
    };
//-----------------------------------------------------------------------------
/// @brief ����������� ����������.
///
/// ���������� ��� ����������� ���������� ������ ���������� � ��������������
/// �������. ������ ������� ������ ������ �� ������. 
class dev_stub : public char_state_device,
    public DO_device,
    public AO_device
    {
    public:
        float get_value();

        int set_value( float new_value );

        int get_state();

        void on();

        void off();

        int set_state( int new_state );

        int parse_cmd( char *buff );
    };
//-----------------------------------------------------------------------------
/// @brief ���������� � ����� ���������� �������.
///
/// ��� ����� ���� ������, �����, ����� ����������...
class DO_1 : public char_state_device,
    public DO_device,
    public wago_device
    {
    public:
        float get_value();

        int set_value( float new_value );

        int get_state();

        void on();

        void off();

        int set_state( int new_state );

        int parse_cmd( char *buff  );

        int load( file *cfg_file );

        void print() const;

    private:
        enum CONSTANTS
            {
            DO_INDEX = 0,
            };
    };
//-----------------------------------------------------------------------------
/// @brief �������.
///
class counter : public u_int_4_state_device,
    public AI_device,
    public wago_device
    {
    public:        
        float get_value();

        int set_value( float new_value );

        int get_state();

        void on();

        void off();

        int set_state( int new_state );

        int parse_cmd( char *buff  );

        int load( file *cfg_file );
        
        void print() const;

        u_int_4 get_u_int_4_state();

    private:
        enum CONSTANTS
            {
            AI_INDEX = 0,
            };
    };
//-----------------------------------------------------------------------------
/// @brief �������� ���������.
///
/// �������� ���������� ��� ���� ����������� �������.
class device_manager
    {
    public:
        int load_from_cfg_file( file *cfg_file );

        DO_device* get_V( int number );

        static device_manager* get_instance();

        static void set_instance( device_manager* new_instance );

    private:
        dev_stub stub;

        struct range
            {
            int start_pos;
            int end_pos;
            };

        range dev_types_ranges[ device::C_DEVICE_TYPE_CNT ];

        /// @brief ��������� ���������� �� ��� ������.
        ///
        int get_device( device::DEVICE_TYPE dev_type, u_int dev_number );

        int    devices_count;
        device **project_devices;

        static device_manager* instance;
    };
//-----------------------------------------------------------------------------
/// ��������� ������� �� ��� ������.
#define V device_manager::get_instance()->get_V
//-----------------------------------------------------------------------------
#endif // PAC_DEVICES_H
