/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package testyeelight;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.persistence.Transient;

/**
 *
 * @author GUOWEI
 */
@Entity
@Table(name = "ttllf_cx")
@NamedQueries({
    @NamedQuery(name = "TtllfCx.findAll", query = "SELECT t FROM TtllfCx t"),
    @NamedQuery(name = "TtllfCx.findByPkCx", query = "SELECT t FROM TtllfCx t WHERE t.pkCx = :pkCx"),
    @NamedQuery(name = "TtllfCx.findByCode", query = "SELECT t FROM TtllfCx t WHERE t.code = :code"),
    @NamedQuery(name = "TtllfCx.findByName", query = "SELECT t FROM TtllfCx t WHERE t.name = :name"),
    @NamedQuery(name = "TtllfCx.findByVbillcode", query = "SELECT t FROM TtllfCx t WHERE t.vbillcode = :vbillcode"),
    @NamedQuery(name = "TtllfCx.findByDbilldate", query = "SELECT t FROM TtllfCx t WHERE t.dbilldate = :dbilldate"),
    @NamedQuery(name = "TtllfCx.findByPkCj", query = "SELECT t FROM TtllfCx t WHERE t.pkCj = :pkCj"),
    @NamedQuery(name = "TtllfCx.findByBillmaker", query = "SELECT t FROM TtllfCx t WHERE t.billmaker = :billmaker"),
    @NamedQuery(name = "TtllfCx.findByDmakedate", query = "SELECT t FROM TtllfCx t WHERE t.dmakedate = :dmakedate"),
    @NamedQuery(name = "TtllfCx.findByApprover", query = "SELECT t FROM TtllfCx t WHERE t.approver = :approver"),
    @NamedQuery(name = "TtllfCx.findByTaudittime", query = "SELECT t FROM TtllfCx t WHERE t.taudittime = :taudittime"),
    @NamedQuery(name = "TtllfCx.findByPkGroup", query = "SELECT t FROM TtllfCx t WHERE t.pkGroup = :pkGroup"),
    @NamedQuery(name = "TtllfCx.findByPkOrg", query = "SELECT t FROM TtllfCx t WHERE t.pkOrg = :pkOrg"),
    @NamedQuery(name = "TtllfCx.findByPkOrgV", query = "SELECT t FROM TtllfCx t WHERE t.pkOrgV = :pkOrgV"),
    @NamedQuery(name = "TtllfCx.findByPkBilltype", query = "SELECT t FROM TtllfCx t WHERE t.pkBilltype = :pkBilltype"),
    @NamedQuery(name = "TtllfCx.findByBilltypecode", query = "SELECT t FROM TtllfCx t WHERE t.billtypecode = :billtypecode"),
    @NamedQuery(name = "TtllfCx.findByPkBusitype", query = "SELECT t FROM TtllfCx t WHERE t.pkBusitype = :pkBusitype"),
    @NamedQuery(name = "TtllfCx.findByFstatusflag", query = "SELECT t FROM TtllfCx t WHERE t.fstatusflag = :fstatusflag"),
    @NamedQuery(name = "TtllfCx.findByFpfstatusflag", query = "SELECT t FROM TtllfCx t WHERE t.fpfstatusflag = :fpfstatusflag"),
    @NamedQuery(name = "TtllfCx.findByVnote", query = "SELECT t FROM TtllfCx t WHERE t.vnote = :vnote"),
    @NamedQuery(name = "TtllfCx.findByDef1", query = "SELECT t FROM TtllfCx t WHERE t.def1 = :def1"),
    @NamedQuery(name = "TtllfCx.findByDef2", query = "SELECT t FROM TtllfCx t WHERE t.def2 = :def2"),
    @NamedQuery(name = "TtllfCx.findByDef3", query = "SELECT t FROM TtllfCx t WHERE t.def3 = :def3"),
    @NamedQuery(name = "TtllfCx.findByDef4", query = "SELECT t FROM TtllfCx t WHERE t.def4 = :def4"),
    @NamedQuery(name = "TtllfCx.findByDef5", query = "SELECT t FROM TtllfCx t WHERE t.def5 = :def5"),
    @NamedQuery(name = "TtllfCx.findByDef6", query = "SELECT t FROM TtllfCx t WHERE t.def6 = :def6"),
    @NamedQuery(name = "TtllfCx.findByDef7", query = "SELECT t FROM TtllfCx t WHERE t.def7 = :def7"),
    @NamedQuery(name = "TtllfCx.findByDef8", query = "SELECT t FROM TtllfCx t WHERE t.def8 = :def8"),
    @NamedQuery(name = "TtllfCx.findByDef9", query = "SELECT t FROM TtllfCx t WHERE t.def9 = :def9"),
    @NamedQuery(name = "TtllfCx.findByDef10", query = "SELECT t FROM TtllfCx t WHERE t.def10 = :def10"),
    @NamedQuery(name = "TtllfCx.findByCreator", query = "SELECT t FROM TtllfCx t WHERE t.creator = :creator"),
    @NamedQuery(name = "TtllfCx.findByCreationtime", query = "SELECT t FROM TtllfCx t WHERE t.creationtime = :creationtime"),
    @NamedQuery(name = "TtllfCx.findByModifier", query = "SELECT t FROM TtllfCx t WHERE t.modifier = :modifier"),
    @NamedQuery(name = "TtllfCx.findByModifiedtime", query = "SELECT t FROM TtllfCx t WHERE t.modifiedtime = :modifiedtime"),
    @NamedQuery(name = "TtllfCx.findByTs", query = "SELECT t FROM TtllfCx t WHERE t.ts = :ts"),
    @NamedQuery(name = "TtllfCx.findByDr", query = "SELECT t FROM TtllfCx t WHERE t.dr = :dr")})
public class TtllfCx implements Serializable {
    @Transient
    private PropertyChangeSupport changeSupport = new PropertyChangeSupport(this);
    private static final long serialVersionUID = 1L;
    @Id
    @Basic(optional = false)
    @Column(name = "pk_cx")
    private String pkCx;
    @Column(name = "code")
    private String code;
    @Column(name = "name")
    private String name;
    @Column(name = "vbillcode")
    private String vbillcode;
    @Column(name = "dbilldate")
    private String dbilldate;
    @Column(name = "pk_cj")
    private String pkCj;
    @Column(name = "billmaker")
    private String billmaker;
    @Column(name = "dmakedate")
    private String dmakedate;
    @Column(name = "approver")
    private String approver;
    @Column(name = "taudittime")
    private String taudittime;
    @Column(name = "pk_group")
    private String pkGroup;
    @Column(name = "pk_org")
    private String pkOrg;
    @Column(name = "pk_org_v")
    private String pkOrgV;
    @Column(name = "pk_billtype")
    private String pkBilltype;
    @Column(name = "billtypecode")
    private String billtypecode;
    @Column(name = "pk_busitype")
    private String pkBusitype;
    @Column(name = "fstatusflag")
    private Integer fstatusflag;
    @Column(name = "fpfstatusflag")
    private Integer fpfstatusflag;
    @Column(name = "vnote")
    private String vnote;
    @Column(name = "def1")
    private String def1;
    @Column(name = "def2")
    private String def2;
    @Column(name = "def3")
    private String def3;
    @Column(name = "def4")
    private String def4;
    @Column(name = "def5")
    private String def5;
    @Column(name = "def6")
    private String def6;
    @Column(name = "def7")
    private String def7;
    @Column(name = "def8")
    private String def8;
    @Column(name = "def9")
    private String def9;
    @Column(name = "def10")
    private String def10;
    @Column(name = "creator")
    private String creator;
    @Column(name = "creationtime")
    private String creationtime;
    @Column(name = "modifier")
    private String modifier;
    @Column(name = "modifiedtime")
    private String modifiedtime;
    @Column(name = "ts")
    private String ts;
    @Column(name = "dr")
    private Short dr;

    public TtllfCx() {
    }

    public TtllfCx(String pkCx) {
        this.pkCx = pkCx;
    }

    public String getPkCx() {
        return pkCx;
    }

    public void setPkCx(String pkCx) {
        String oldPkCx = this.pkCx;
        this.pkCx = pkCx;
        changeSupport.firePropertyChange("pkCx", oldPkCx, pkCx);
    }

    public String getCode() {
        return code;
    }

    public void setCode(String code) {
        String oldCode = this.code;
        this.code = code;
        changeSupport.firePropertyChange("code", oldCode, code);
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        String oldName = this.name;
        this.name = name;
        changeSupport.firePropertyChange("name", oldName, name);
    }

    public String getVbillcode() {
        return vbillcode;
    }

    public void setVbillcode(String vbillcode) {
        String oldVbillcode = this.vbillcode;
        this.vbillcode = vbillcode;
        changeSupport.firePropertyChange("vbillcode", oldVbillcode, vbillcode);
    }

    public String getDbilldate() {
        return dbilldate;
    }

    public void setDbilldate(String dbilldate) {
        String oldDbilldate = this.dbilldate;
        this.dbilldate = dbilldate;
        changeSupport.firePropertyChange("dbilldate", oldDbilldate, dbilldate);
    }

    public String getPkCj() {
        return pkCj;
    }

    public void setPkCj(String pkCj) {
        String oldPkCj = this.pkCj;
        this.pkCj = pkCj;
        changeSupport.firePropertyChange("pkCj", oldPkCj, pkCj);
    }

    public String getBillmaker() {
        return billmaker;
    }

    public void setBillmaker(String billmaker) {
        String oldBillmaker = this.billmaker;
        this.billmaker = billmaker;
        changeSupport.firePropertyChange("billmaker", oldBillmaker, billmaker);
    }

    public String getDmakedate() {
        return dmakedate;
    }

    public void setDmakedate(String dmakedate) {
        String oldDmakedate = this.dmakedate;
        this.dmakedate = dmakedate;
        changeSupport.firePropertyChange("dmakedate", oldDmakedate, dmakedate);
    }

    public String getApprover() {
        return approver;
    }

    public void setApprover(String approver) {
        String oldApprover = this.approver;
        this.approver = approver;
        changeSupport.firePropertyChange("approver", oldApprover, approver);
    }

    public String getTaudittime() {
        return taudittime;
    }

    public void setTaudittime(String taudittime) {
        String oldTaudittime = this.taudittime;
        this.taudittime = taudittime;
        changeSupport.firePropertyChange("taudittime", oldTaudittime, taudittime);
    }

    public String getPkGroup() {
        return pkGroup;
    }

    public void setPkGroup(String pkGroup) {
        String oldPkGroup = this.pkGroup;
        this.pkGroup = pkGroup;
        changeSupport.firePropertyChange("pkGroup", oldPkGroup, pkGroup);
    }

    public String getPkOrg() {
        return pkOrg;
    }

    public void setPkOrg(String pkOrg) {
        String oldPkOrg = this.pkOrg;
        this.pkOrg = pkOrg;
        changeSupport.firePropertyChange("pkOrg", oldPkOrg, pkOrg);
    }

    public String getPkOrgV() {
        return pkOrgV;
    }

    public void setPkOrgV(String pkOrgV) {
        String oldPkOrgV = this.pkOrgV;
        this.pkOrgV = pkOrgV;
        changeSupport.firePropertyChange("pkOrgV", oldPkOrgV, pkOrgV);
    }

    public String getPkBilltype() {
        return pkBilltype;
    }

    public void setPkBilltype(String pkBilltype) {
        String oldPkBilltype = this.pkBilltype;
        this.pkBilltype = pkBilltype;
        changeSupport.firePropertyChange("pkBilltype", oldPkBilltype, pkBilltype);
    }

    public String getBilltypecode() {
        return billtypecode;
    }

    public void setBilltypecode(String billtypecode) {
        String oldBilltypecode = this.billtypecode;
        this.billtypecode = billtypecode;
        changeSupport.firePropertyChange("billtypecode", oldBilltypecode, billtypecode);
    }

    public String getPkBusitype() {
        return pkBusitype;
    }

    public void setPkBusitype(String pkBusitype) {
        String oldPkBusitype = this.pkBusitype;
        this.pkBusitype = pkBusitype;
        changeSupport.firePropertyChange("pkBusitype", oldPkBusitype, pkBusitype);
    }

    public Integer getFstatusflag() {
        return fstatusflag;
    }

    public void setFstatusflag(Integer fstatusflag) {
        Integer oldFstatusflag = this.fstatusflag;
        this.fstatusflag = fstatusflag;
        changeSupport.firePropertyChange("fstatusflag", oldFstatusflag, fstatusflag);
    }

    public Integer getFpfstatusflag() {
        return fpfstatusflag;
    }

    public void setFpfstatusflag(Integer fpfstatusflag) {
        Integer oldFpfstatusflag = this.fpfstatusflag;
        this.fpfstatusflag = fpfstatusflag;
        changeSupport.firePropertyChange("fpfstatusflag", oldFpfstatusflag, fpfstatusflag);
    }

    public String getVnote() {
        return vnote;
    }

    public void setVnote(String vnote) {
        String oldVnote = this.vnote;
        this.vnote = vnote;
        changeSupport.firePropertyChange("vnote", oldVnote, vnote);
    }

    public String getDef1() {
        return def1;
    }

    public void setDef1(String def1) {
        String oldDef1 = this.def1;
        this.def1 = def1;
        changeSupport.firePropertyChange("def1", oldDef1, def1);
    }

    public String getDef2() {
        return def2;
    }

    public void setDef2(String def2) {
        String oldDef2 = this.def2;
        this.def2 = def2;
        changeSupport.firePropertyChange("def2", oldDef2, def2);
    }

    public String getDef3() {
        return def3;
    }

    public void setDef3(String def3) {
        String oldDef3 = this.def3;
        this.def3 = def3;
        changeSupport.firePropertyChange("def3", oldDef3, def3);
    }

    public String getDef4() {
        return def4;
    }

    public void setDef4(String def4) {
        String oldDef4 = this.def4;
        this.def4 = def4;
        changeSupport.firePropertyChange("def4", oldDef4, def4);
    }

    public String getDef5() {
        return def5;
    }

    public void setDef5(String def5) {
        String oldDef5 = this.def5;
        this.def5 = def5;
        changeSupport.firePropertyChange("def5", oldDef5, def5);
    }

    public String getDef6() {
        return def6;
    }

    public void setDef6(String def6) {
        String oldDef6 = this.def6;
        this.def6 = def6;
        changeSupport.firePropertyChange("def6", oldDef6, def6);
    }

    public String getDef7() {
        return def7;
    }

    public void setDef7(String def7) {
        String oldDef7 = this.def7;
        this.def7 = def7;
        changeSupport.firePropertyChange("def7", oldDef7, def7);
    }

    public String getDef8() {
        return def8;
    }

    public void setDef8(String def8) {
        String oldDef8 = this.def8;
        this.def8 = def8;
        changeSupport.firePropertyChange("def8", oldDef8, def8);
    }

    public String getDef9() {
        return def9;
    }

    public void setDef9(String def9) {
        String oldDef9 = this.def9;
        this.def9 = def9;
        changeSupport.firePropertyChange("def9", oldDef9, def9);
    }

    public String getDef10() {
        return def10;
    }

    public void setDef10(String def10) {
        String oldDef10 = this.def10;
        this.def10 = def10;
        changeSupport.firePropertyChange("def10", oldDef10, def10);
    }

    public String getCreator() {
        return creator;
    }

    public void setCreator(String creator) {
        String oldCreator = this.creator;
        this.creator = creator;
        changeSupport.firePropertyChange("creator", oldCreator, creator);
    }

    public String getCreationtime() {
        return creationtime;
    }

    public void setCreationtime(String creationtime) {
        String oldCreationtime = this.creationtime;
        this.creationtime = creationtime;
        changeSupport.firePropertyChange("creationtime", oldCreationtime, creationtime);
    }

    public String getModifier() {
        return modifier;
    }

    public void setModifier(String modifier) {
        String oldModifier = this.modifier;
        this.modifier = modifier;
        changeSupport.firePropertyChange("modifier", oldModifier, modifier);
    }

    public String getModifiedtime() {
        return modifiedtime;
    }

    public void setModifiedtime(String modifiedtime) {
        String oldModifiedtime = this.modifiedtime;
        this.modifiedtime = modifiedtime;
        changeSupport.firePropertyChange("modifiedtime", oldModifiedtime, modifiedtime);
    }

    public String getTs() {
        return ts;
    }

    public void setTs(String ts) {
        String oldTs = this.ts;
        this.ts = ts;
        changeSupport.firePropertyChange("ts", oldTs, ts);
    }

    public Short getDr() {
        return dr;
    }

    public void setDr(Short dr) {
        Short oldDr = this.dr;
        this.dr = dr;
        changeSupport.firePropertyChange("dr", oldDr, dr);
    }

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (pkCx != null ? pkCx.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof TtllfCx)) {
            return false;
        }
        TtllfCx other = (TtllfCx) object;
        if ((this.pkCx == null && other.pkCx != null) || (this.pkCx != null && !this.pkCx.equals(other.pkCx))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "testyeelight.TtllfCx[ pkCx=" + pkCx + " ]";
    }

    public void addPropertyChangeListener(PropertyChangeListener listener) {
        changeSupport.addPropertyChangeListener(listener);
    }

    public void removePropertyChangeListener(PropertyChangeListener listener) {
        changeSupport.removePropertyChangeListener(listener);
    }
    
}
