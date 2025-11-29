import { useQuery } from "@tanstack/react-query"
import { GetUser } from "@/api/auth"

const useAuthQuery = () => { 
    const token = localStorage.getItem("token");

    return useQuery({
        queryKey: ["auth", "user"],
        queryFn: GetUser,
        enabled: !!token,
        retry: false,
        staleTime: 1000 * 60 * 50
    });

}

export default useAuthQuery;