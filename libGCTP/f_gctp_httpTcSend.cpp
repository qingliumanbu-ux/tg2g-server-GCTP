/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      178773
Version:     1.0
Date:        2021-05-25 14:06:36
Description: HTTP接口数据发送
**************************************************/

#include "CDynaTable.h"

BM2_FUNCTION_EXPORT
int f_gctp_httpTcSend(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString tcNo, CString colResFlag, CString colMessage)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CString svcName = "";
	CString outSystem = "";
	CString tcType = "";
	CDbCommand cmd(conn);
	CDynaTable tcData("TGCTP21", conn);

	try
	{
		sqlstr = "SELECT code_desc_2_content,code_desc_3_content FROM tep0002 WHERE code_class = 'M00N' AND code = '" + tcNo + "'";
		PrintLog("sqlstr", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			svcName = cmd.GetString(1);
			outSystem = cmd.GetString(2);
		}
		cmd.Close();

		if (svcName.Trim() == "")
		{
			strcpy(s.msg, "没有配置接口地址");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		
		tcData.AddHoldColName("RESUME_SEQ_NO");

		PrintLog("tcType", tcType);
		if (tcType == "1")
		{
			tcData.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			tcData.SetColVal("TC_NO", tcNo);
			tcData.SetColVal("RESUME_SEQ_NO", GetTrackSeqNo("MM00_RESUME_SEQ_NO", 20, conn));
			tcData.Print();

			EIClass bcls_rec_tc;
			EIClass bcls_ret_tc;
			CDataRow& drAc = tcData.GetDataRow();
			SetErpTcDataTable(tcNo, "0", drAc, &bcls_rec_tc, conn);

			PrintLog("Tables[0]");
			PrintDataTable(bcls_rec_tc.Tables[0]);

			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				tcData.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tcData.SetColVal("TC_NO", tcNo);
				tcData.SetColVal("SEQ_NO", (CDecimal)i + 1);
				tcData.Print("SEQ_NO");

				if (i > 0)
				{
					tcData.SetColVal("RESUME_SEQ_NO", GetTrackSeqNo("MM00_RESUME_SEQ_NO", 20, conn));
				}

				CDataRow& drAc = tcData.GetDataRow();
				SetErpTcDataTable(tcNo, "1", drAc, &bcls_rec_tc, conn);
			}

			PrintLog("Tables[1]");
			PrintDataTable(bcls_rec_tc.Tables[1]);

			f_epex_call_rest_svc(conn, outSystem, svcName, &bcls_rec_tc, &bcls_ret_tc);
			PrintDataTable(bcls_ret_tc.Tables[0]);

			ei_sys eiRet;
			bcls_ret_tc.GetSYS(&eiRet);
			Log::Trace("", __FUNCTION__, "flag = [{0}], msg = [{1}]", eiRet.flag, eiRet.msg);

			tcData.SetColVal("SEND_FLAG", "1");
			tcData.SetColVal("SEND_TIME", CDateTime::Now().ToString("yyyyMMddHHmmss"));
			tcData.SetColVal("RES_FLAG", (CDecimal)eiRet.flag);
			tcData.SetColVal("RES_REASON", eiRet.msg);

			//tcData.SetColVal("RES_FLAG", GetColValueC(bcls_ret->Tables[0], 0, colResFlag));
			//tcData.SetColVal("RES_REASON", GetColValueC(bcls_ret->Tables[0], 0, colMessage));

			if (tcData.GetColValString("RES_FLAG") != "0")
			{
				//throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (tcData.Insert() < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		else
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				tcData.MergeFrom(bcls_rec->Tables[0].Rows[0]);
				tcData.SetColVal("TC_NO", tcNo);
				tcData.SetColVal("RESUME_SEQ_NO", GetTrackSeqNo("MM00_RESUME_SEQ_NO", 20, conn));
				tcData.CopyColVal("PROD_SEQ_NO", "RESUME_SEQ_NO");

				EIClass bcls_rec_tc;
				EIClass bcls_ret_tc;
				CDataRow& drAc = tcData.GetDataRow();
				SetErpTcDataTable(tcNo, "", drAc, &bcls_rec_tc, conn);

				PrintDataTable(bcls_rec_tc.Tables[0]);
				f_epex_call_rest_svc(conn, outSystem, svcName, &bcls_rec_tc, &bcls_ret_tc);
				PrintDataTable(bcls_ret_tc.Tables[0]);

				ei_sys eiRet;
				bcls_ret_tc.GetSYS(&eiRet);
				Log::Trace("", __FUNCTION__, "flag = [{0}], msg = [{1}]", eiRet.flag, eiRet.msg);

				tcData.SetColVal("SEND_FLAG", "1");
				tcData.SetColVal("SEND_TIME", CDateTime::Now().ToString("yyyyMMddHHmmss"));
				tcData.SetColVal("RES_FLAG", (CDecimal)eiRet.flag);
				tcData.SetColVal("RES_REASON", eiRet.msg);

				//tcData.SetColVal("RES_FLAG", GetColValueC(bcls_ret->Tables[0], 0, colResFlag));
				//tcData.SetColVal("RES_REASON", GetColValueC(bcls_ret->Tables[0], 0, colMessage));

				if (tcData.GetColValString("RES_FLAG") != "0")
				{
					//throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (tcData.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
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

	if (doFlag == 0)
	{
		CTransactionManager::Commit(0);
		CTransactionManager::Begin(0, 0);
	}
	else
	{
		CTransactionManager::Abort(0);
		CTransactionManager::Begin(0, 0);
	}

	return doFlag;
}


