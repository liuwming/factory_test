/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package testyeelight;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Embeddable;

/**
 *
 * @author GUOWEI
 */
@Embeddable
public class KbChipnoinfoPK implements Serializable {
    @Basic(optional = false)
    @Column(name = "CHIPNO")
    private String chipno;
    @Basic(optional = false)
    @Column(name = "SUSER")
    private String suser;

    public KbChipnoinfoPK() {
    }

    public KbChipnoinfoPK(String chipno, String suser) {
        this.chipno = chipno;
        this.suser = suser;
    }

    public String getChipno() {
        return chipno;
    }

    public void setChipno(String chipno) {
        this.chipno = chipno;
    }

    public String getSuser() {
        return suser;
    }

    public void setSuser(String suser) {
        this.suser = suser;
    }

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (chipno != null ? chipno.hashCode() : 0);
        hash += (suser != null ? suser.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof KbChipnoinfoPK)) {
            return false;
        }
        KbChipnoinfoPK other = (KbChipnoinfoPK) object;
        if ((this.chipno == null && other.chipno != null) || (this.chipno != null && !this.chipno.equals(other.chipno))) {
            return false;
        }
        if ((this.suser == null && other.suser != null) || (this.suser != null && !this.suser.equals(other.suser))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "testyeelight.KbChipnoinfoPK[ chipno=" + chipno + ", suser=" + suser + " ]";
    }
    
}
