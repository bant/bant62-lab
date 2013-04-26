//========================================================================
// File Name    : TinyI2CMaster.c
//
// Title        : ATtiny�p USI���g����I2C�h���C�o
// Revision     : 0.11
// Notes        :
// Target MCU   : AVR ATtiny series
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// ????/??/??   �����V����  soft_I2C.c�J������
// 2013/04/10   �΂��      �C������
// 2013/04/26   �΂��      ���W�X�^����֐��ǉ�&�ύX
//------------------------------------------------------------------------
// This code is distributed under Apache License 2.0 License
//		which can be found at http://www.apache.org/licenses/
//========================================================================

/* Includes -------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/delay.h>
#include "TinyI2CMaster.h"

/* local define ---------------------------------------------------------*/
#define NO_MORE_READ    0
#define MORE_READ       1

#define TOGL_USICR      ((0<<USISIE)|(0<<USIOIE)|(1<<USIWM1)|(0<<USIWM0)|(1<<USICS1)|(0<<USICS0)|(1<<USICLK)|(1<<USITC))
#define TEMP_USISR_8    ((1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|(0x0<<USICNT0))
#define TEMP_USISR_1    ((1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|(0xE<<USICNT0))

/* local typedef --------------------------------------------------------*/
/* local macro ----------------------------------------------------------*/
/* local variables ------------------------------------------------------*/
/* local function prototypes --------------------------------------------*/

/* [�������炪���V����̃\�[�X] ======================================== */

//========================================================================
//  USI�C���^�t�F�[�X�̏�����(�Ή��|�[�g��������)
//------------------------------------------------------------------------
// ����: �Ȃ�
// �ߒl: �Ȃ�
//========================================================================
void TinyI2C_Master_init( void )
{
    USIDR = 0xFF;                          //release data reg

    PORT_USI  |=(1<<PIN_USI_SCL)|(1<<PIN_USI_SDA); //�s���͓����v���A�b�v
    DDR_USI   |=(1<<PIN_USI_SCL)|(1<<PIN_USI_SDA); //�ŏ��͏o�͕����ɂ��Ă���

    // 2wire mode  �\�t�g�E�G�A�N���b�N �ŏ��̓G�b�W�Ńg�O�����Ȃ�(USITC=0)
    USICR = TOGL_USICR;
    USICR &= ~(1<<USITC);

    //�X�e�C�^�X���W�X�^�͂��ׂăN���A
    USISR = TEMP_USISR_8;
}

//========================================================================
//  �X�^�[�g�R���f�B�V�������M
//------------------------------------------------------------------------
// ����: �Ȃ�
// �ߒl: 0=����I���@����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t TinyI2C_start( void )
{
#ifdef NOISE_TESTING                                // Test if any unexpected conditions have arrived prior to this execution.
    if( USISR & (1<<USISIF) )
    {
        return TINYI2C_UNKNOWN_START;
    }

    if( USISR & (1<<USIPF) )
    {
        return TINYI2C_UNKNOWN_STOP;
    }

    if( USISR & (1<<USIDC) )
    {
        return TINYI2C_DATA_COLLISION;
    }
#endif

    PORT_USI |= (1<<PIN_USI_SCL);               //set SCL 1
    while( !(PIN_USI & (1<<PIN_USI_SCL)) );     //wait SCL high
    _delay_us(T2_TWI);
    PORT_USI &= ~(1<<PIN_USI_SDA);              // Force SDA LOW
    _delay_us(T4_TWI);
    PORT_USI &= ~(1<<PIN_USI_SCL);              //Pull SCL low
    PORT_USI |=  (1<<PIN_USI_SDA);              //Release SDA

#ifdef SIGNAL_VERIFY
    if(!(USISR & (1<<USISIF)))
    {
        return TINYI2C_MISS_START_COND;
    }
#endif

    return TINYI2C_NO_ERROR;
}


//========================================================================
//  �X�g�b�v�R���f�B�V�����̑��M
//------------------------------------------------------------------------
// ����: �Ȃ�
// �ߒl: 0=����I���@����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t TinyI2C_stop( void )
{
    uint8_t retval;

    retval = TINYI2C_NO_ERROR;

    PORT_USI &= ~(1<<PIN_USI_SDA);              //pull SDA low
    PORT_USI |=  (1<<PIN_USI_SCL);              //Release SCL
    while( !(PIN_USI & (1<<PIN_USI_SCL)) );     //wait SCL high
    _delay_us(T2_TWI);
    PORT_USI |= (1<<PIN_USI_SDA);               // set SDA in High(Z)
    _delay_us(T4_TWI);
#ifdef SIGNAL_VERIFY
    if(!(USISR & (1<<USIPF)) )
    {
        retval = TINYI2C_MISS_STOP_COND;
    }
#endif
    USISR |= (1<<USIPF);                        //clear stop condition

    return retval;
}


//========================================================================
//  1�o�C�g�ǂݍ���(�ǂݍ��ݐ錾�̂��Ɓj
//------------------------------------------------------------------------
// ����: uint8_t more: more �� MORE_READ�̂Ƃ�ACK���M�A����ȊO��NACK���M
// �ߒl: �ǂݍ��܂ꂽ1�o�C�g�̃f�[�^
//========================================================================
uint8_t TinyI2C_read( uint8_t more )
{
    uint8_t data;

    DDR_USI &= ~(1<<PIN_USI_SDA);               //enable SDA as input
    data = TinyI2C_Transfer(TEMP_USISR_8);      //read 8 bits
    if(more == MORE_READ)                       //if read more
        USIDR = 0x00;                           //set ACK
    else
        USIDR =0xFF;                            // NACK
    TinyI2C_Transfer(TEMP_USISR_1);             // generate (N)ACK (1bit)

    return data;
}


//========================================================================
//  1�o�C�g��������
//------------------------------------------------------------------------
// ����: uint8_t data �������ރf�[�^
// �ߒl: 0=����I���@����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t TinyI2C_write( uint8_t data )
{
    uint8_t retval;

    retval = TINYI2C_NO_ERROR;                  //

    PORT_USI &= ~(1<<PIN_USI_SCL);              //Pull SCL low
    USIDR = data;                               //set Data
    TinyI2C_Transfer(TEMP_USISR_8);
    DDR_USI &= ~(1<<PIN_USI_SDA);               //���͂ɐ؂芷��
    if(TinyI2C_Transfer(TEMP_USISR_1) & 0x01)
    {
        retval = TINYI2C_SLAVE_NACK;            //listen to response
    }

    return retval;
}

//========================================================================
//  USI�C���^�t�F�[�X�ɂ��f�[�^����M 8�r�b�g��1�r�b�g������
//------------------------------------------------------------------------
// ����: uint8_t data: ����M�f�[�^
// �ߒl: 0=����I���@����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t TinyI2C_Transfer( uint8_t data )
{
    uint8_t retval;

    USISR = data;
    do
    {
        _delay_us(T2_TWI);
        USICR = TOGL_USICR;                     //generate positive SCL edge
        while(!(PIN_USI &(1<<PIN_USI_SCL)) );   //wait for SCL to go high
        _delay_us(T4_TWI);
        USICR = TOGL_USICR;
    }
    while(!(USISR &(1<<USIOIF)) );              //4bit�J�E���^�I����҂�

    _delay_us(T2_TWI);
    retval = USIDR;                             //�ǂݍ��݂̂Ƃ��̓f�[�^������
    USIDR = 0xFF;                               //Release SDA
    DDR_USI |=(1<<PIN_USI_SDA);                 //�o�̓��[�h�ɕς���

    return retval;
}
/* =========================================[�����܂ł����V����̃\�[�X] */

/* [��������΂�Ƃ̃\�[�X] ============================================ */
#ifdef USE_READ_WRITE_REPEAT
//========================================================================
//  �f�[�^�A���ǂݍ���
//------------------------------------------------------------------------
// ����: uint8_t slave_7bit_addr : �^�[�Q�b�g��7�r�b�g�A�h���X
//       void* data              : �ǂݍ��ރf�[�^
//       int size                : �ǂݍ��ރf�[�^�T�C�Y
//       uint8_t send_stop       : ��0�Ȃ�Ǎ����STOP�R���f�B�V�������M����
// �ߒl: 0=����I���@����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t TinyI2C_read_data(uint8_t slave_7bit_addr, void* data, int size, uint8_t send_stop )
{
    register int    i;
    uint8_t status;
    uint8_t *p;

    p= data;
    for(i = 0; i < RETRY; i++)
    {
        // �X�^�[�g�R���f�B�V�������s
        status = TinyI2C_start();
        if (status != TINYI2C_NO_ERROR)
        {
			// ���g���C����
			if(status == TINYI2C_UNKNOWN_START)
           	{
            	continue;
			}
			else if(status == TINYI2C_UNKNOWN_STOP)
           	{
            	continue;
			}
            else if(status == TINYI2C_DATA_COLLISION)
           	{
            	continue;
			}
			else
         	{
                break;
			}
        }

        // �}�X�^�[�̎�M�錾
        status = TinyI2C_write((slave_7bit_addr<<1) | 0x01);
        if (status !=  TINYI2C_NO_ERROR)
        {
            break;
        }

        for (; size > 0; --size)
        {
            if (size==1)
            {
                *p = TinyI2C_read(NO_MORE_READ);
            }
            else
            {
                *p++ = TinyI2C_read(MORE_READ);
            }
        }
        status = TINYI2C_NO_ERROR;
        break;
    }

    // Send stop condition
    if (send_stop != 0)
    {
        status = TinyI2C_stop( );
    }

    return status;
}

//========================================================================
//  �f�[�^�A����������
//------------------------------------------------------------------------
// ����: uint8_t slave_7bit_addr : �^�[�Q�b�g��7�r�b�g�A�h���X
//       void* data              : �������ރf�[�^
//       int size                : �������ރf�[�^�T�C�Y
//       uint8_t send_stop       : ��0�Ȃ�Ǎ����STOP�R���f�B�V�������M����
// �ߒl: 0=����I���@����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t TinyI2C_write_data(uint8_t slave_7bit_addr, void* data, int size, uint8_t send_stop)
{
    register int    i;
    uint8_t status;
    uint8_t *p;

    p = data;
    for (i = 0; i< RETRY; i++)
    {
        // �X�^�[�g�R���f�B�V�������s
        status = TinyI2C_start();
        if (status !=  TINYI2C_NO_ERROR)
        {
			// ���g���C����
			if(status == TINYI2C_UNKNOWN_START)
           	{
            	continue;
			}
			else if(status == TINYI2C_UNKNOWN_STOP)
           	{
            	continue;
			}
            else if(status == TINYI2C_DATA_COLLISION)
           	{
            	continue;
			}
			else
         	{
                break;
			}
        }

        // �}�X�^�[�̑��M�錾
        status = TinyI2C_write((slave_7bit_addr<<1) | 0x00);
        if (status !=  TINYI2C_NO_ERROR)
        {
            break;
        }

        for(; size > 0; --size)
        {
            status = TinyI2C_write( *p++ );
            if (status !=  TINYI2C_NO_ERROR)
            {
                break;
            }
        }
        break;
    }

    // Send stop condition
    if (send_stop != 0)
    {
        status = TinyI2C_stop( );
    }

    return status;
}

#ifdef USE_READ_WRITE_REGISTER
//========================================================================
//  ���W�X�^�ǂݍ���
//------------------------------------------------------------------------
// ����: uint8_t slave_7bit_addr : �^�[�Q�b�g��7�r�b�g�A�h���X
//       uint8_t mem_addr        : ���W�X�^�̃������A�h���X
//       uint8_t* data           : �ǂݍ��ރf�[�^
// �ߒl: 0=����I���@����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t TinyI2C_readReg( uint8_t slave_7bit_addr, uint8_t mem_addr, uint8_t *data )
{
    uint8_t status;

    // �A�h���X���M
    status = TinyI2C_write_data(slave_7bit_addr, &mem_addr, 1, NO_SEND_STOP);
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    return TinyI2C_read_data(slave_7bit_addr, data, 1, SEND_STOP);
}

//========================================================================
//  ���W�X�^�}�X�N��������
//------------------------------------------------------------------------
// ����: uint8_t slave_7bit_addr : �^�[�Q�b�g��7�r�b�g�A�h���X
//       uint8_t mem_addr        : ���W�X�^�̃������A�h���X
//       uint8_t mask            : �ݒ肷��r�b�g�̃}�X�N�f�[�^
//       uint8_t set_bit         : �ݒ�f�[�^
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t TinyI2C_masksetRegBit( uint8_t slave_7bit_addr, uint8_t mem_addr, uint8_t mask, uint8_t set_bit )
{
    uint8_t data[2];
    uint8_t status;

    data[0] = mem_addr;
    status = TinyI2C_readReg(slave_7bit_addr, data[0], &data[1] );
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }
    data[1] &= ~mask;
    data[1] |= set_bit;

    return TinyI2C_write_data(slave_7bit_addr, data, 2, SEND_STOP);
}

//========================================================================
//  ����r�b�g�ݒ�
//------------------------------------------------------------------------
// ����: uint8_t slave_7bit_addr : �^�[�Q�b�g��7�r�b�g�A�h���X
//       uint8_t mem_addr        : ���W�X�^�̃������A�h���X
//       uint8_t set_bit         : �ݒ肷��r�b�g�̈ʒu
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t TinyI2C_setRegBit( uint8_t slave_7bit_addr, uint8_t mem_addr, uint8_t set_bit )
{
    uint8_t data[2];
    uint8_t status;

    data[0] = mem_addr;
    status = TinyI2C_readReg( slave_7bit_addr, data[0], &data[1] );
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    data[1] |= set_bit;

    return TinyI2C_write_data( slave_7bit_addr, data, 2, SEND_STOP );
}

//========================================================================
//  ����r�b�g�N���A
//------------------------------------------------------------------------
// ����: uint8_t slave_7bit_addr : �^�[�Q�b�g��7�r�b�g�A�h���X
//       uint8_t mem_addr        : ���W�X�^�̃������A�h���X
//       uint8_t clear_bit       : �N���A����r�b�g�̈ʒu
// �ߒl: 0=����I�� ����ȊOI2C�ʐM�G���[
//========================================================================
uint8_t TinyI2C_clearRegBit( uint8_t slave_7bit_addr, uint8_t mem_addr, uint8_t clear_bit )
{
    uint8_t data[2];
    uint8_t status;

    data[0] = mem_addr;
    status = TinyI2C_readReg( slave_7bit_addr, data[0], &data[1] );
    if(status != TINYI2C_NO_ERROR)
    {
        return status;
    }

    data[1] &= ~clear_bit;

    return TinyI2C_write_data( slave_7bit_addr, data, 2, SEND_STOP );
}


#endif  /* USE_READ_WRITE_REGISTER */
#endif  /* USE_READ_WRITE_REPEAT */

/* =============================================[�����܂ł΂�Ƃ̃\�[�X] */
