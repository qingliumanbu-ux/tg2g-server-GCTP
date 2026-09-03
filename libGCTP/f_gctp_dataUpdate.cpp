/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      admin
Version:     1.0
Date:        2018-12-19 08:36:57
Description: 数据修改函数
**************************************************/

#include "CDynaTable2.h"

BM2_FUNCTION_IMPORT
int f_gctp_getConfigData(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_gctp_dataUpdate(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CDecimal buttonNo = 0;
	CString sTableType = "";
	CString sTableName = "";
	CString sDataSource = "";
	CString sDataSet = "";
	CString sEventId = "";
	CString sEventDesc = "";
	CString sLogTableName = "";

	CDataTable dtMain;
	CDataTable dtMainItem;
	CDataTable dtConditionMain;
	CDataTable dtCallButton;

	try
	{
		if (bcls_rec->Tables.Contains("DS_MAIN") && bcls_rec->Tables.Contains("DS_ITEM") &&
			bcls_rec->Tables.Contains("OPERATE_DO"))
		{
			dtMain.Copy(bcls_rec->Tables["DS_MAIN"]);
			dtMainItem.Copy(bcls_rec->Tables["DS_ITEM"]);
			dtCallButton.Copy(bcls_rec->Tables["OPERATE_DO"]);
		}
		else if (GetColValueC(bcls_rec->Tables[0], 0, "CFGITM_NAME").Trim() != "")
		{
			doFlag = f_gctp_getConfigData(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			dtMain.Copy(bcls_ret->Tables["DS_MAIN"]);
			dtMainItem.Copy(bcls_ret->Tables["DS_ITEM"]);
			dtCallButton.Copy(bcls_ret->Tables["OPERATE_DO"]);
		}
		else
		{
			strcpy(s.msg, "传入数据不完整");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sTableType = dtMain.Rows[0]["TABLE_TYPE"].ToString();
		if (sTableType.Trim() == "")
		{
			strcpy(s.msg, "没有配置数据表类型");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		PrintLog("sTableType", sTableType);

		sTableName = dtMain.Rows[0]["TABLE_NAME"].ToString();
		if (sTableName.Trim() == "")
		{
			strcpy(s.msg, "没有配置数据表名");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		PrintLog("sTableName", sTableName);

		if (dtCallButton.Rows.get_Count() > 0)
		{
			sEventId = GetColValueC(dtCallButton, 0, "EVENT_ID").Trim();
			sEventDesc = GetColValueC(dtCallButton, 0, "EVENT_DESC").Trim();
			sLogTableName = GetColValueC(dtCallButton, 0, "LOG_TABLE_NAME").Trim();
		}

		buttonNo = dtCallButton.Rows[0]["OPERATE_BUTTON"].ToDecimal();
		PrintLog("buttonNo", buttonNo);

		sDataSource = "DEFAULT_ENABLE_STATE_" + buttonNo.ToString();
		sDataSet = "ENABLE_CTRL_STR_" + buttonNo.ToString();

		PrintLog("sDataSource", sDataSource);
		PrintLog("sDataSet", sDataSet);

		//定义数据表
		CDynaTable2 dynaTable(sTableName, conn);

		if (sTableType == "2")
		{

		}
		else
		{
			//获取前台传入新增数据
			if (bcls_rec->Tables.Contains("DATA_UPD"))
			{
				dynaTable.CopyFrom(bcls_rec->Tables["DATA_UPD"]);
			}
			else
			{
				dynaTable.CopyFrom(bcls_rec->Tables[0]);
			}

			for (int i = 0; i < dtMainItem.Rows.get_Count(); i++)
			{
				CString colName = dtMainItem.Rows[i]["COLUMN_NAME"].ToString();
				if (dtMainItem.Rows[i]["KEYWORD_FLAG"].ToString() == "1")
				{
					dynaTable.AddFilterColName(colName);
					PrintLog("FilterColName", colName);
				}

				if (GetColValueC(dtMainItem, i, sDataSource) == "3")
				{
					if (dtMainItem.Rows[i]["DATA_TYPE"].ToString() == "C")
					{
						if (GetColValueC(dtMainItem, i, sDataSource).Trim() == "")
						{
							dynaTable.SetColValAllRow(colName, " ");
						}
						else
						{
							dynaTable.SetColValAllRow(colName, GetColValueC(dtMainItem, i, sDataSource));
						}

					}
					else
					{
						if (GetColValueC(dtMainItem, i, sDataSource).Trim() == "")
						{
							dynaTable.SetColValAllRow(colName, (CDecimal)0);
						}
						else
						{
							dynaTable.SetColValAllRow(colName, GetColValueD(dtMainItem, i, sDataSet));
						}
					}
				}
				if (GetColValueC(dtMainItem, i, sDataSource) == "4"  && GetColValueC(dtMainItem, i, sDataSource).Trim() != "")
				{
					for (int j = 0; j < dynaTable.GetRowCount(); j++)
					{
						dynaTable.CopyColVal(colName, GetColValueC(dtMainItem, i, sDataSource).Trim(), j);
					}
				}
				else if (GetColValueC(dtMainItem, i, sDataSource) == "5")
				{
					PrintLog("Seqence,colName", colName);
					for (int j = 0; j < dynaTable.GetRowCount(); j++)
					{
						dynaTable.SetColVal(colName, GetSeqence(GetColValueC(dtMainItem, i, sDataSource), 0, conn), j);
					}
				}
				else if (GetColValueC(dtMainItem, i, sDataSource) == "6")
				{
					dynaTable.SetColValAllRow(colName, GetTrackSeqNo(GetColValueC(dtMainItem, i, sDataSource), 0, conn));
				}
				else if (GetColValueC(dtMainItem, i, sDataSource) == "7")
				{
					dynaTable.SetColValAllRow(colName, CDateTime::Now().ToString("yyyyMMddHHmmss"));
				}
				else if (GetColValueC(dtMainItem, i, sDataSource) == "8")
				{
					dynaTable.SetColValAllRow(colName, (CString)s.userid);
				}

				if (GetColValueC(dtMainItem, i, sDataSource) != "0")
				{
					dynaTable.AddUpdateColName(colName);
				}
			}

			//if (bcls_rec->Tables.Contains("UPD_COLUMN"))
			//{
			//	sBlockName2 = "UPD_COLUMN";
			//}
			//else
			//{
			//	sBlockName2 = sBlockName1;
			//}

			//for (int i = 0; i < bcls_rec->Tables[sBlockName2].Columns.get_Count(); i++)
			//{
			//	CString colName = bcls_rec->Tables[sBlockName2].Columns[i].get_ColumnName();
			//	dynaTable.AddUpdateColName(colName);
			//}

			if (dynaTable.Update() < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (sEventId != "" && sLogTableName != "")
			{
				PrintLog("sEventId", sEventId);
				PrintLog("sEventDesc", sEventDesc);
				PrintLog("sLogTableName", sLogTableName);

				CDynaTable2 logData(sLogTableName, conn);
				logData.CopyFrom(dynaTable.GetDataTable());
				for (int i = 0; i < logData.GetRowCount(); i++)
				{
					logData.SetColVal("EVENT_ID", sEventId, i);
					logData.SetColVal("EVENT_NAME", sEventDesc, i);
					logData.SetColVal("EVENT_DESC", sEventDesc, i);
					logData.SetColVal("FUNC_ID", (CString)s.svc_name, i);
					logData.SetColVal("FORM_NAME", (CString)s.formname, i);
					logData.SetColVal("RESUME_SEQ_NO", GetTrackSeqNo("MMTP_RESUME_SEQ_NO", 20, conn), i);
				}

				if (logData.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		}

		dynaTable.CopyTo(bcls_ret->Tables[0]);
		PrintDataTable(bcls_ret->Tables[0]);

		if (GetColValueC(bcls_rec->Tables[0], 0, "DEBUG_FLAG") == "1")
		{
			strcpy(s.msg, "调试成功");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}