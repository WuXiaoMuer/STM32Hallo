#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  
#include "i2c.h"

u8 OLED_GRAM[128][8];
static uint16_t oled_dev_addr = OLED_I2C_ADDR;

// ���Ժ�����������
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		OLED_WR_Byte(0xA6, OLED_CMD); // ������ʾ
	if(i==1)
		OLED_WR_Byte(0xA7, OLED_CMD); // ��ɫ��ʾ
}

// ��Ļ��ת180�ȣ�������
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
	{
		OLED_WR_Byte(0xC8, OLED_CMD); // ������ʾ
		OLED_WR_Byte(0xA1, OLED_CMD);
	}
	if(i==1)
	{
		OLED_WR_Byte(0xC0, OLED_CMD); // ��ת��ʾ
		OLED_WR_Byte(0xA0, OLED_CMD);
	}
}

// �����޸ģ�Ӳ��I2C2д�ֽڣ��滻ԭSPI�߼���
// dat: Ҫ���͵�����/����
// mode: OLED_CMD(0)=д���OLED_DATA(1)=д����
void OLED_WR_Byte(u8 dat, u8 mode)
{
    u8 tx_buf[2];
    // SSD1306 I2CЭ�飺��һ���ֽ�=D/Cλ + �ӻ���ַ
    // D/Cλ��0=���1=���ݣ��ӻ���ַ=OLED_I2C_ADDR��0x78/0x7A��
    tx_buf[0] = mode ? 0x40 : 0x00; // D/Cλ���ϲ�������֡��һ���ֽڣ�
    tx_buf[1] = dat;                // ʵ��Ҫ���͵�����/����
    
    // Ӳ��I2C2���ͣ�����ʽ����ʱ500ms��
    // hi2c2: CubeMX���ɵ�I2C2�����PB10=SCL, PB3=SDA��
    HAL_I2C_Master_Transmit(&hi2c2, oled_dev_addr, tx_buf, 2, 500);
}

// ����OLED��ʾ��������
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D, OLED_CMD); // ��ɱ�ʹ��
	OLED_WR_Byte(0x14, OLED_CMD); // ������ɱ�
	OLED_WR_Byte(0xAF, OLED_CMD); // ������Ļ
}

// �ر�OLED��ʾ��������
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D, OLED_CMD); // ��ɱ�ʹ��
	OLED_WR_Byte(0x10, OLED_CMD); // �رյ�ɱ�
	OLED_WR_Byte(0xAE, OLED_CMD); // �ر���Ļ
}

// �����Դ浽OLED����������ͨ�Ų��ΪI2C��
void OLED_Refresh(void)
{
	u8 i, n;
	for(i=0; i<8; i++)  // 8ҳ��ÿҳ8�У���64�У�
	{
	   OLED_WR_Byte(0xb0+i, OLED_CMD); // ����ҳ��ַ��Y����
	   OLED_WR_Byte(0x00, OLED_CMD);   // �����е͵�ַ��X�����4λ��
	   OLED_WR_Byte(0x10, OLED_CMD);   // �޸����иߵ�ַ��Ϊ0x10��ԭ0x00����
	   
	   // ����������ҳ���ݣ��Ż�������I2C���ô�����
	   u8 tx_buf[128];
	   for(n=0; n<128; n++)
		   tx_buf[n] = OLED_GRAM[n][i];
	   // �ȷ���"д����"ָ��ͷ���ٷ�����ҳ����
       HAL_I2C_Mem_Write(&hi2c2, oled_dev_addr, 0x40, I2C_MEMADD_SIZE_8BIT, tx_buf, 128, 500);
  }
}

// ����������������
void OLED_Clear(void)
{
	u8 i, n;
	for(i=0; i<8; i++)
		for(n=0; n<128; n++)
			OLED_GRAM[n][i] = 0; // �����������
	OLED_Refresh(); // ������ʾ
}

// ���㣨������
void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
	u8 i, m, n;
	if(x>127 || y>63) return; // �߽���
	
	i = y/8;    // ��������ҳ
	m = y%8;    // ����ҳ����
	n = 1<<m;   // ����λ����
	
	if(t) 
		OLED_GRAM[x][i] |= n;  // ������
	else 
		OLED_GRAM[x][i] &= ~n; // ��յ�
}

// ���ߣ�������
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode)
{
	u16 t; 
	int xerr=0, yerr=0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2-x1; // ������������ 
	delta_y = y2-y1;
	uRow = x1;       // �����������
	uCol = y1;
	
	if(delta_x>0) incx=1;    // ���õ������� 
	else if(delta_x==0) incx=0; // ��ֱ�� 
	else {incx=-1; delta_x=-delta_x;}
	
	if(delta_y>0) incy=1;
	else if(delta_y==0) incy=0; // ˮƽ�� 
	else {incy=-1; delta_y=-delta_y;}
	
	if(delta_x>delta_y) distance=delta_x; // ѡȡ�������������� 
	else distance=delta_y;
	
	for(t=0; t<distance+1; t++)
	{
		OLED_DrawPoint(uRow, uCol, mode); // ����
		xerr += delta_x;
		yerr += delta_y;
		
		if(xerr>distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if(yerr>distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

// ��Բ��������
void OLED_DrawCircle(u8 x, u8 y, u8 r)
{
	int a, b, num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b, 1);
        OLED_DrawPoint(x - a, y - b, 1);
        OLED_DrawPoint(x - a, y + b, 1);
        OLED_DrawPoint(x + a, y + b, 1);
 
        OLED_DrawPoint(x + b, y + a, 1);
        OLED_DrawPoint(x + b, y - a, 1);
        OLED_DrawPoint(x - b, y - a, 1);
        OLED_DrawPoint(x - b, y + a, 1);
        
        a++;
        num = (a*a + b*b) - r*r; // ���㻭�ĵ���Բ�ĵľ���
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}

// ��ʾ�ַ���������
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1, u8 mode)
{
	u8 i, m, temp, size2, chr1;
	u8 x0=x, y0=y;
	
	if(size1==8) size2=6;
	else size2 = (size1/8 + ((size1%8)?1:0)) * (size1/2);  // �����ֽ���
	chr1 = chr - ' ';  // ����ƫ�ƺ��ֵ
	
	for(i=0; i<size2; i++)
	{
		if(size1==8) temp = asc2_0806[chr1][i];   // 0806����
		else if(size1==12) temp = asc2_1206[chr1][i]; // 1206����
		else if(size1==16) temp = asc2_1608[chr1][i]; // 1608����
		else if(size1==24) temp = asc2_2412[chr1][i]; // 2412����
		else return;
		
		for(m=0; m<8; m++)
		{
			if(temp&0x01) OLED_DrawPoint(x, y, mode);
			else OLED_DrawPoint(x, y, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if((size1!=8) && ((x-x0)==size1/2))
		{
			x = x0;
			y0 = y0+8;
		}
		y = y0;
  }
}

// ��ʾ�ַ�����������
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1, u8 mode)
{
	while((*chr>=' ') && (*chr<='~')) // ���˷Ƿ��ַ�
	{
		OLED_ShowChar(x, y, *chr, size1, mode);
		if(size1==8) x+=6;
		else x+=size1/2;
		chr++;
  }
}

// �����㣨������
u32 OLED_Pow(u8 m, u8 n)
{
	u32 result=1;
	while(n--) result*=m;
	return result;
}

// ��ʾ���֣�������
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size1, u8 mode)
{
	u8 t, temp, m=0;
	if(size1==8) m=2;
	
	for(t=0; t<len; t++)
	{
		temp = (num / OLED_Pow(10, len-t-1)) % 10;
		if(temp==0)
			OLED_ShowChar(x+(size1/2+m)*t, y, '0', size1, mode);
		else 
			OLED_ShowChar(x+(size1/2+m)*t, y, temp+'0', size1, mode);
  }
}

// ��ʾ���֣���������oledfont.h��Hzk1/Hzk2�����飩
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1, u8 mode)
{
	u8 m, temp;
	u8 x0=x, y0=y;
	u16 i, size3 = (size1/8 + ((size1%8)?1:0)) * size1;  // �����ֽ���
	
	for(i=0; i<size3; i++)
	{
		if(size1==16) temp = Hzk1[num][i]; // 16*16����
		else if(size1==24) temp = Hzk2[num][i]; // 24*24����
		else if(size1==32) temp = Hzk3[num][i]; // 32*32����
		else if(size1==64) temp = Hzk4[num][i]; // 64*64����
		else return;
		
		for(m=0; m<8; m++)
		{
			if(temp&0x01) OLED_DrawPoint(x, y, mode);
			else OLED_DrawPoint(x, y, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if((x-x0)==size1)
		{
			x = x0;
			y0 = y0+8;
		}
		y = y0;
	}
}

// OLED��ʼ�����޸ģ�ɾ��SPI/GPIO��ʼ��������I2C+��λ�߼���
void OLED_Init(void)
{
    if (HAL_I2C_IsDeviceReady(&hi2c2, oled_dev_addr, 5, 100) != HAL_OK)
    {
        oled_dev_addr = 0x7A;
        if (HAL_I2C_IsDeviceReady(&hi2c2, oled_dev_addr, 5, 100) != HAL_OK)
        {
            return;
        }
    }

    OLED_WR_Byte(0xAE, OLED_CMD);
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0x10, OLED_CMD);
    OLED_WR_Byte(0x40, OLED_CMD);
    OLED_WR_Byte(0xB0, OLED_CMD);
    OLED_WR_Byte(0x81, OLED_CMD);
    OLED_WR_Byte(0xFF, OLED_CMD);
    OLED_WR_Byte(0xA1, OLED_CMD);
    OLED_WR_Byte(0xA6, OLED_CMD);
    OLED_WR_Byte(0xA8, OLED_CMD);
    OLED_WR_Byte(0x3F, OLED_CMD);
    OLED_WR_Byte(0xC8, OLED_CMD);
    OLED_WR_Byte(0xD3, OLED_CMD);
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0xD5, OLED_CMD);
    OLED_WR_Byte(0x80, OLED_CMD);
    OLED_WR_Byte(0xD9, OLED_CMD);
    OLED_WR_Byte(0xF1, OLED_CMD);
    OLED_WR_Byte(0xDA, OLED_CMD);
    OLED_WR_Byte(0x12, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD);
    OLED_WR_Byte(0x40, OLED_CMD);
    OLED_WR_Byte(0x8D, OLED_CMD);
    OLED_WR_Byte(0x14, OLED_CMD);

    OLED_Clear();
    OLED_WR_Byte(0xAF, OLED_CMD);
}
