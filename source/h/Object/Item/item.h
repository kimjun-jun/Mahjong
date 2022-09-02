/**
* @file item.h
* @brief NiceShot(3D)��ԃQ�[��
* @author �L�����W����
*/
#pragma once

#include "../../../h/Object/ObjectClass/Interface/interface.h"
#include "../../../h/Object/ObjectClass/Instance/instance.h"
#include "../../../h/Object/ObjectClass/StandardComponent/Model/Model.h"
#include "../../../h/Object/ObjectClass/StandardComponent/TRANSFORM/TransForm.h"
#include "../../../h/Object/ObjectClass/StandardComponent/UseCheck/UseCheck.h"
#include "../../../h/Object/ObjectClass/StandardComponent/Posture/Posture.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
constexpr float ITEM_BIG_SCL{ 2.0f };			//�A�C�e���T�C�Y�����l
constexpr float ITEM_SMALL_SCL{ 0.03f };			//�A�C�e���擾�オ�A�C�e�������񂾂񏬂����Ȃ�l
constexpr float ITEM_DELETE_SCL{ 0.5f };			//�A�C�e����������l

//*****************************************************************************
// �N���X��`
//*****************************************************************************
/**
*�@@class ITEM_PARAMETER_ALL
*�@@brief 
*/
class ITEM_PARAMETER_ALL
{
public:
	ITEM_PARAMETER_ALL() {
		nIdxShadow = -1; GetPlayerType = 0; LinkShadowPos = VEC3_ALL0; GettingSignal = false; GettingSignalEnd = false;
		CollisionFieldEnd = false; NetUse = false; NetGetItemFlag = false; NetGetItemFlagOld = false; ShadowPosSignal = false;
		eType = ITEM_TYPE_NONE;
	}
	~ITEM_PARAMETER_ALL() {}

	int		nIdxShadow;				//!< �eID
	int		GetPlayerType;			//!< �ǂ̃v���C���[���擾������
	bool	GettingSignal;			//!< �v���C���[���A�C�e�����擾�������ǂ����@�^:�擾���@�U:�擾���Ă��Ȃ�
	bool	GettingSignalEnd;		//!< �v���C���[���A�C�e�����擾�I���������ǂ����@�^:�I���@�U:�I�����Ă��Ȃ��A�������͎擾���Ă��Ȃ�
	bool	CollisionFieldEnd;		//!< �n�`�Ƃ̓����蔻�肪�I��������ǂ���
	bool	ShadowPosSignal;		//!< �e�̍����v�Z�t���O

	bool	NetUse;
	bool	NetGetItemFlag;			//!< �A�C�e�����擾����1�t���[��������true�ɂ���
	bool	NetGetItemFlagOld;		//!< �A�C�e�����擾����1�t���[��������true�ɂ��� ��r�l

	D3DXVECTOR3		LinkShadowPos;	//!< �e�̍��W
	eITEM_TYPE		eType;			//!< ���

};

/**
*�@@class ITEM_PARAMETER_ONE
*�@@brief GAMEOBJECT�h���N���X
*/
class ITEM_PARAMETER_ONE
{
public:
	ITEM_PARAMETER_ONE() { GoukeiDrop = 0, Droptime = -1; 
	MaxSize = D3DXVECTOR3(ITEM_BIG_SCL, ITEM_BIG_SCL, ITEM_BIG_SCL); MinSize = D3DXVECTOR3(ITEM_DELETE_SCL, ITEM_DELETE_SCL, ITEM_DELETE_SCL);}
	~ITEM_PARAMETER_ONE() {}

	int	GoukeiDrop;			//!< �h���b�v���v
	int	Droptime;			//!< �o������܂ł̎���
	D3DXVECTOR3	MaxSize;	//!< �ő�T�C�Y
	D3DXVECTOR3	MinSize;	//!< �ŏ��T�C�Y
};

/**
*�@@class ITEM
*�@@brief GAMEOBJECT�h���N���X
*/
class ITEM : private GAME_OBJECT_INTERFACE_SUMMRY
{
public:
	ITEM();		//!< �f�[�^�ǂݍ��݁@������
	~ITEM();	//!< �폜

	void Addressor(GAME_OBJECT_INSTANCE *obj) override;	//!< �A�h���b�T�[
	void Init(void) override;			//!< ������
	void InitNet(void)override {};		//!< �������l�b�g�ΐ�p�ɕύX���K�v�ȂƂ��Ŏg�p
	void Update(void)override;			//!< �X�V
	void Draw(void)override;			//!< �`��

	ITEM_PARAMETER_ALL ItemParaAll[OBJECT_ITEM_MAX];	//!< �e�C���X�^���X�ɕK�v�ȃf�[�^�Q
	iUseCheck iUseType[OBJECT_ITEM_MAX];		//!< �g�p���
	TRANSFORM Transform[OBJECT_ITEM_MAX];		//!< �g�����X�t�H�[�����
	POSTURE	PostureVec[OBJECT_ITEM_MAX];	//!< �p���x�N�g��
	void SetInstance(D3DXVECTOR3 pos, D3DXVECTOR3 scl, D3DXVECTOR3 rot, eITEM_TYPE eType);
	void SetInstance(int Index, D3DXVECTOR3 pos, D3DXVECTOR3 scl, D3DXVECTOR3 rot, eITEM_TYPE eType);
	void ReleaseInstance(int nIdxItem);

private:
	MODELATTRIBUTE model;				//!< ���f�����@�}�e���A���Ⓒ�_���Ȃǁ@�����g�p����Ȃ炱����z��
	TEXTURE tex[ITEM_TYPE_MAX];			//!< �e�N�X�`�����@�����g�p����Ȃ炱����z�񉻁@ITEMTYPE_MAX
	VTXBUFFER vtx[OBJECT_ITEM_MAX];		//!< ���_���@�����g�p����Ȃ炱����z��
	ITEM_PARAMETER_ONE ItemParaOne;		//!< �}�l�[�W���[�ɕK�v�ȃf�[�^�Q

	//------���N���X�̃A�h���X
	PLAYER *pplayer;
	SHADOW *pshadow;
	MySOCKET *pmysocket;

	void	GettingItem(int nIdxItem);

	const char *c_aFileNameModel[ITEM_TYPE_MAX] =
	{
		"../data/MODEL/tikeiItem.x",		// �n�`�ό`�A�C�e��
		"../data/MODEL/lifeItem.x",			// ���C�t��
		"../data/MODEL/sensyaItem.x",		// ��ԕό`�A�C�e��
		"../data/MODEL/bulletItem.x",		// �o���b�g�A�C�e��
		"../data/MODEL/speedItem.x",		// �X�s�[�h�A�b�v�A�C�e��
		"../data/MODEL/cameraItem.x",		// �����o�b�N�J�����A�C�e��
		"../data/MODEL/kiriItem.x",			// ���A�C�e��
	};

	const char *c_aFileNameTex[ITEM_TYPE_MAX] =
	{
		"../data/MODEL/landmark_aogashima.png",			// �n�`�ό`�A�C�e��
		"../data/MODEL/life000.png",					// ���C�t��
		"../data/MODEL/war_sensya_noman.png",			// ��ԕό`�A�C�e��
		"../data/MODEL/bullettex.png",					// �o���b�g�A�C�e��
		"../data/MODEL/1213810.png",					// �X�s�[�h�A�b�v�A�C�e��
		"../data/MODEL/mark_camera_satsuei_ok.png",		// �����o�b�N�J�����A�C�e��
		"../data/MODEL/yama_kiri.png",					// ���A�C�e��
	};

} ;
