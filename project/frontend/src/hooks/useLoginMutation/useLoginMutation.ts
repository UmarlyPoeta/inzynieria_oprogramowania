
import { useMutation, useQueryClient } from "@tanstack/react-query";
import { SignIn } from "@/api/auth";

const useLoginMutation = () => {
  const qc = useQueryClient();

  return useMutation({
    mutationFn: SignIn,
    onSuccess: (data) => {
      localStorage.setItem("token", data.token);

      qc.invalidateQueries({ queryKey: ["auth", "user"] });
    },
  });
};

export default useLoginMutation;
