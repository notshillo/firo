#ifndef FIRO_BATCHPROOF_CONTAINER_H
#define FIRO_BATCHPROOF_CONTAINER_H

#include <memory>
#include "chain.h"
#include "sigma/coinspend.h"
#include "liblelantus/joinsplit.h"

extern CChain chainActive;

class BatchProofContainer {
public:
    static BatchProofContainer* get_instance();

    struct SigmaProofData {
        SigmaProofData() : sigmaProof(0, 0), coinSerialNumber(uint64_t(0)), fPadding(0), anonymitySetSize(0) {}
        SigmaProofData(const sigma::SigmaPlusProof<Scalar, GroupElement>& sigmaProof_,
                       const Scalar& coinSerialNumber_,
                       bool fPadding_,
                       size_t anonymitySetSize_)
                       : sigmaProof(sigmaProof_),
                       coinSerialNumber(coinSerialNumber_),
                       fPadding(fPadding_),
                       anonymitySetSize(anonymitySetSize_) {}

        sigma::SigmaPlusProof<Scalar, GroupElement> sigmaProof;
        Scalar coinSerialNumber;
        bool fPadding;
        size_t anonymitySetSize;
    };

    struct LelantusSigmaProofData {
        LelantusSigmaProofData(const lelantus::SigmaExtendedProof& lelantusSigmaProof_,
                               const Scalar& serialNumber_,
                               const Scalar& challenge_,
                               size_t anonymitySetSize_)
                               : lelantusSigmaProof(lelantusSigmaProof_),
                               serialNumber(serialNumber_),
                               challenge(challenge_),
                               anonymitySetSize(anonymitySetSize_) {}

        lelantus::SigmaExtendedProof lelantusSigmaProof;
        Scalar serialNumber;
        Scalar challenge;
        size_t anonymitySetSize;
    };

    void init();

    void finalize();

    void verify();

    void add(sigma::CoinSpend* spend,
             bool fPadding,
             int group_id,
             size_t setSize,
             bool fStartSigmaBlacklist);

    void add(lelantus::JoinSplit* joinSplit,
             const std::map<uint32_t, size_t>& setSizes,
             const Scalar& challenge,
             bool fStartLelantusBlacklist);

    void removeSigma(const sigma::spend_info_container& spendSerials);
    void removeLelantus(std::unordered_map<Scalar, int> spentSerials);
    void erase(std::vector<LelantusSigmaProofData>* vProofs, const Scalar& serial);

    void batch_sigma();
    void batch_lelantus();

public:
    bool fCollectProofs = 0;

private:
    static std::unique_ptr<BatchProofContainer> instance;
    // temp containers, to forget in case block connection fails
    // map (denom, id) to (sigma proof, serial, set size)
    std::map<std::pair<sigma::CoinDenomination, std::pair<int, bool>>, std::vector<SigmaProofData>> tempSigmaProofs;
    // map ((id, afterFixes), fIsSigmaToLelantus) to (sigma proof, serial, set size, challenge)
    std::map<std::pair<std::pair<uint32_t, bool>, bool>, std::vector<LelantusSigmaProofData>> tempLelantusSigmaProofs;

    // containers to keep proofs for batching
    std::map<std::pair<sigma::CoinDenomination, std::pair<int, bool>>, std::vector<SigmaProofData>> sigmaProofs;
    std::map<std::pair<std::pair<uint32_t, bool>, bool>, std::vector<LelantusSigmaProofData>> lelantusSigmaProofs;
};

#endif //FIRO_BATCHPROOF_CONTAINER_H
